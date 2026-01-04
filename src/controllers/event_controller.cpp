/**
 * @file event_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Event Controller Implementation (Safe Blocking Long Polling)
 * @version 0.3.9
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/event_controller.hpp"
#include "models/event_model.hpp"
#include "models/user_model.hpp"
#include "middleware/auth_middleware.hpp"
#include "services/notification_service.hpp"
#include "utils/env_loader.hpp"

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <QDir>
#include <QFile>
#include <QUuid>
#include <iostream>

// --- Helpers (SSE / Long Polling) ---
std::mutex event_mutex;
std::condition_variable event_cv;

struct LatestEventData {
    long long generation = 0;
    crow::json::wvalue payload;
} latestEvent;

void broadcastNewEvent(const Event& evt) {
    {
        std::lock_guard<std::mutex> lock(event_mutex);
        latestEvent.generation++;
        crow::json::wvalue msg;
        msg["type"] = "NEW_EVENT";
        msg["groupId"] = evt.groupId.toStdString();
        msg["bakerName"] = evt.bakerName.toStdString();
        msg["date"] = evt.date.toStdString();
        latestEvent.payload = std::move(msg);
    }
    event_cv.notify_all();
}

namespace rz {
namespace controller {

void EventController::registerRoutes(crow::App<rz::middleware::AuthMiddleware> &app, service::NotificationService* notifyService) {

    // --- STATIC ROUTE für Bilder ---
    CROW_ROUTE(app, "/api/uploads/<string>")
    ([](crow::response& res, std::string filename){
        // Lade Pfad aus ENV (Default: public/uploads)
        std::string baseDir = rz::utils::EnvLoader::get("UPLOAD_DIR", "public/uploads").toStdString();

        // Security: Directory Traversal verhindern
        if (filename.find("..") != std::string::npos) {
            res.code = 403;
            res.end();
            return;
        }

        std::string fullPath = baseDir + "/" + filename;
        res.set_static_file_info(fullPath);
        res.end();
    });

    // 0. SSE Stream
    CROW_ROUTE(app, "/api/events/stream")
    ([&](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "text/event-stream");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");

        long long myGen;
        {
            std::lock_guard<std::mutex> lock(event_mutex);
            myGen = latestEvent.generation;
        }
        std::unique_lock<std::mutex> lock(event_mutex);
        bool hasNewEvent = event_cv.wait_for(lock, std::chrono::seconds(29), [&]{
            return latestEvent.generation > myGen;
        });
        if (hasNewEvent) {
            std::string body = "data: " + latestEvent.payload.dump() + "\n\n";
            res.write(body);
        } else {
            res.write(": keepalive\n\n");
        }
        res.end();
    });

    // 1. GET /api/events
    CROW_ROUTE(app, "/api/events")
    ([&](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        auto start = req.url_params.get("start");
        auto end = req.url_params.get("end");

        if (!start || !end) return crow::response(400, "Missing params");

        auto events = Event::getRange(start, end, ctx.currentUser.userId);
        crow::json::wvalue result = crow::json::wvalue::list();
        int i = 0;
        for (const auto &e : events) {
            result[i++] = e.toJson();
        }
        return crow::response(result);
    });

    // 2. POST /api/events (Erstellen)
    CROW_ROUTE(app, "/api/events")
    .methods(crow::HTTPMethod::POST)([&, notifyService](const crow::request &req) {
        const auto &ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

        crow::multipart::message msg(req);
        QString date, description, savedFileName;

        QString uploadDir = rz::utils::EnvLoader::get("UPLOAD_DIR", "public/uploads");

        for (const auto &part : msg.parts) {
            const auto &disp = part.headers.find("Content-Disposition");
            if (disp != part.headers.end()) {
                auto params = disp->second.params;
                if (params.count("name")) {
                    std::string name = params["name"];

                    if (name == "event") {
                        auto json = crow::json::load(part.body);
                        if (json) {
                            date = QString::fromStdString(json["date"].s());
                            if (json.has("description"))
                                description = QString::fromStdString(json["description"].s());
                        }
                    }
                    else if (name == "image" && part.body.size() > 0) {
                        QString ext = ".jpg";
                        QString fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ext;

                        QDir().mkpath(uploadDir);
                        QFile file(uploadDir + "/" + fileName);
                        if (file.open(QIODevice::WriteOnly)) {
                            file.write(part.body.data(), part.body.size());
                            file.close();
                            savedFileName = fileName;
                        }
                    }
                }
            }
        }

        if (date.isEmpty()) return crow::response(400, "Date required");

        Event e;
        e.date = date;
        e.description = description;
        // Wir setzen photoPath hier temporär für das Event-Objekt
        e.photoPath = savedFileName;

        if (e.create(ctx.currentUser.userId)) {
            // Wenn ein Bild hochgeladen wurde, registrieren wir es auch in der Gallery Tabelle
            if (!savedFileName.isEmpty()) {
                Event::uploadPhoto(e.id, ctx.currentUser.userId, savedFileName);
            }

            broadcastNewEvent(e);

            if (notifyService && !e.groupId.isEmpty()) {
                auto members = User::getAll(e.groupId);
                std::vector<QString> de, en;
                for (const auto& u : members) {
                    if (u.id == ctx.currentUser.userId) continue;
                    if (!u.is_active) continue;
                    if (u.emailLanguage == "de") de.push_back(u.email);
                    else en.push_back(u.email);
                }
                notifyService->notifyGroupNewEvent(e.groupName, e.bakerName, e.date, de, en);
            }

            crow::json::wvalue res;
            res["message"] = "Event created";
            res["id"] = e.id.toStdString();
            return crow::response(201, res);
        } else {
            return crow::response(500, "Error creating event");
        }
    });

    // 3. GET Single
    CROW_ROUTE(app, "/api/events/<string>")
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        auto evt = Event::getById(QString::fromStdString(eventId), ctx.currentUser.userId);
        if(evt) return crow::response(evt->toJson());
        return crow::response(404);
    });

    // 4. DELETE
    CROW_ROUTE(app, "/api/events/<string>")
    .methods(crow::HTTPMethod::DELETE)
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        if(Event::deleteEvent(QString::fromStdString(eventId), ctx.currentUser.userId)) {
            return crow::response(200);
        }
        return crow::response(403);
    });

    // 5. Rate
    CROW_ROUTE(app, "/api/events/<string>/rate")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        auto json = crow::json::load(req.body);
        if(!json) return crow::response(400);

        int stars = json["stars"].i();
        std::string comment = json.has("comment") ? std::string(json["comment"].s()) : std::string("");

        if(Event::rateEvent(QString::fromStdString(eventId), ctx.currentUser.userId, stars, QString::fromStdString(comment))) {
            return crow::response(200);
        }
        return crow::response(500);
    });

    // 6. ICS
    CROW_ROUTE(app, "/api/events/<string>/ics")
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);
        auto evt = Event::getById(QString::fromStdString(eventId), ctx.currentUser.userId);
        if(!evt) return crow::response(404);

        crow::response res(evt->toIcsString());
        res.set_header("Content-Type", "text/calendar");
        res.set_header("Content-Disposition", "attachment; filename=event.ics");
        return res;
    });

    // 7. Photo Upload (Nachträglich / Gallery)
    CROW_ROUTE(app, "/api/events/<string>/photo")
    .methods(crow::HTTPMethod::POST)([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<rz::middleware::AuthMiddleware>(req);

        auto evt = Event::getById(QString::fromStdString(eventId), ctx.currentUser.userId);
        if (!evt) return crow::response(404, "Event not found");

        // Jeder authentifizierte User (der Zugriff auf das Event hat) darf ein Foto hochladen
        // Die Logik "1 Foto pro User" wird im Model/DB durch Unique Constraint geregelt

        crow::multipart::message msg(req);
        std::string fileContent;

        for (const auto &part : msg.parts) {
             const auto &disp = part.headers.find("Content-Disposition");
             if (disp != part.headers.end()) {
                 auto it = disp->second.params.find("name");
                 if (it != disp->second.params.end() && it->second == "photo") {
                     fileContent = part.body;
                     break;
                 }
             }
        }

        if (fileContent.empty()) return crow::response(400, "No file provided");

        // Pfad aus ENV laden
        QString uploadDir = rz::utils::EnvLoader::get("UPLOAD_DIR", "public/uploads");
        QString ext = ".jpg"; // Vereinfachung
        QString fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ext;

        QDir().mkpath(uploadDir);
        QFile file(uploadDir + "/" + fileName);

        if (file.open(QIODevice::WriteOnly)) {
            file.write(fileContent.data(), fileContent.size());
            file.close();

            // DB Update: Wir rufen uploadPhoto auf (User ID, Event ID, Dateiname)
            if(Event::uploadPhoto(QString::fromStdString(eventId), ctx.currentUser.userId, fileName)) {
                return crow::response(200);
            }
        }

        return crow::response(500, "Could not save file or update DB");
    });
}

} // namespace controller
} // namespace rz

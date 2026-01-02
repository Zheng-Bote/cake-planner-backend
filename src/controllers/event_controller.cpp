/**
 * @file event_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.3.2
 * @date 2026-01-02
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/event_controller.hpp"
#include "models/event_model.hpp"
#include "middleware/auth_middleware.hpp"

#include <mutex>
#include <set>
#include <QDir>
#include <QFile>
#include <QUuid>
#include <iostream>
#include <thread>

// --- Helpers ---

std::mutex sse_mutex;
std::set<crow::response*> sse_connections;

// Helper: Broadcast an alle Clients
void broadcastNewEvent(const Event& evt) {
    std::lock_guard<std::mutex> lock(sse_mutex);

    crow::json::wvalue msg;
    msg["type"] = "NEW_EVENT";
    msg["groupId"] = evt.groupId.toStdString();
    msg["bakerName"] = evt.bakerName.toStdString();
    msg["date"] = evt.date.toStdString();
    std::string body = "data: " + msg.dump() + "\n\n";

    for(auto it = sse_connections.begin(); it != sse_connections.end(); ) {
        (*it)->write(body);
        (*it)->end();
        it = sse_connections.erase(it);
    }
}

// --- Routes ---

void EventController::registerRoutes(crow::App<AuthMiddleware> &app) {

    // ---------------------------------------------------------
    // WICHTIG: Spezifische Routen (wie /stream) MÜSSEN
    // VOR generischen Routen (wie /<string>) registriert werden!
    // ---------------------------------------------------------

    // 0. SSE Stream (Ganz oben!)
    CROW_ROUTE(app, "/api/events/stream")
    ([&](const crow::request& req, crow::response& res){
        res.set_header("Content-Type", "text/event-stream");
        res.set_header("Cache-Control", "no-cache");
        res.set_header("Connection", "keep-alive");

        {
            std::lock_guard<std::mutex> lock(sse_mutex);
            sse_connections.insert(&res);
        }

        // Thread offen halten
        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // 1. GET /api/events (Liste)
    CROW_ROUTE(app, "/api/events")
    ([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);

        auto start = req.url_params.get("start");
        auto end = req.url_params.get("end");

        if (!start || !end) {
            return crow::response(400, "Missing start/end date params");
        }

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
    .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);

        crow::multipart::message msg(req);
        QString date;
        QString description;
        QString savedFileName;

        for (const auto &part : msg.parts) {
            const auto &contentDisposition = part.headers.find("Content-Disposition");
            if (contentDisposition != part.headers.end()) {
                auto params = contentDisposition->second.params;
                if (params.find("name") != params.end()) {
                    std::string name = params["name"];
                    if (name == "date") date = QString::fromStdString(part.body);
                    else if (name == "description") description = QString::fromStdString(part.body);
                    else if (name == "photo" && part.body.size() > 0) {
                        QString ext = ".jpg";
                        QString fileName = QUuid::createUuid().toString(QUuid::WithoutBraces) + ext;
                        QDir().mkpath("data/uploads");
                        QFile file("data/uploads/" + fileName);
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
        e.photoPath = savedFileName;

        if (e.create(ctx.currentUser.userId)) {
            broadcastNewEvent(e); // SSE Trigger
            crow::json::wvalue res;
            res["message"] = "Event created successfully";
            res["id"] = e.id.toStdString();
            return crow::response(201, res);
        } else {
            return crow::response(500, "Could not create event. Group missing?");
        }
    });

    // 3. GET Single Event (Generisch - muss NACH /stream kommen)
    CROW_ROUTE(app, "/api/events/<string>")
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<AuthMiddleware>(req);
        if(ctx.currentUser.userId.isEmpty()) return crow::response(401);

        auto evt = Event::getById(QString::fromStdString(eventId), ctx.currentUser.userId);
        if(evt) return crow::response(evt->toJson());
        return crow::response(404);
    });

    // 4. DELETE Event
    CROW_ROUTE(app, "/api/events/<string>")
    .methods(crow::HTTPMethod::DELETE)
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<AuthMiddleware>(req);
        if(ctx.currentUser.userId.isEmpty()) return crow::response(401);

        if(Event::deleteEvent(QString::fromStdString(eventId), ctx.currentUser.userId)) {
            return crow::response(200);
        }
        return crow::response(403, "Not allowed (not owner or past event)");
    });

    // 5. Rating
    CROW_ROUTE(app, "/api/events/<string>/rate")
    .methods(crow::HTTPMethod::POST)
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<AuthMiddleware>(req);
        auto json = crow::json::load(req.body);
        if(!json) return crow::response(400);

        int stars = json["stars"].i();
        std::string comment = "";
        if (json.has("comment")) {
            comment = json["comment"].s();
        }

        if(Event::rateEvent(QString::fromStdString(eventId), ctx.currentUser.userId, stars, QString::fromStdString(comment))) {
            return crow::response(200);
        }
        return crow::response(500);
    });

    // 6. Download ICS
    CROW_ROUTE(app, "/api/events/<string>/ics")
    ([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<AuthMiddleware>(req);
        if(ctx.currentUser.userId.isEmpty()) return crow::response(401);

        auto evt = Event::getById(QString::fromStdString(eventId), ctx.currentUser.userId);
        if(!evt) return crow::response(404);

        crow::response res(evt->toIcsString());
        res.set_header("Content-Type", "text/calendar");
        res.set_header("Content-Disposition", "attachment; filename=event.ics");
        return res;
    });

    // 7. Upload Photo (Nachträglich)
    CROW_ROUTE(app, "/api/events/<string>/photo")
    .methods(crow::HTTPMethod::POST)([&](const crow::request& req, std::string eventId){
        const auto& ctx = app.get_context<AuthMiddleware>(req);

        crow::multipart::message msg(req);
        std::string fileContent;
        std::string ext = "jpg";

        for (const auto &part : msg.parts) {
            const auto &disp = part.headers.find("Content-Disposition");
            if (disp != part.headers.end()) {
                auto params = disp->second.params;
                if (params.find("name") != params.end() && params["name"] == "photo") {
                    fileContent = part.body;
                    if (params.find("filename") != params.end()) {
                        std::string fn = params["filename"];
                        if (fn.find(".png") != std::string::npos) ext = "png";
                    }
                }
            }
        }

        if (fileContent.empty()) return crow::response(400, "No file provided");

        if(Event::uploadPhoto(QString::fromStdString(eventId), ctx.currentUser.userId, fileContent, ext)) {
            return crow::response(200);
        }
        return crow::response(500);
    });
}

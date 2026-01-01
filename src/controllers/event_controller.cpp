/**
 * @file event_controller.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief No description provided
 * @version 0.1.0
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "controllers/event_controller.hpp"
#include "models/event_model.hpp"
#include <QDir>
#include <QFile>
#include <QUuid>
#include <iostream>

void EventController::registerRoutes(crow::App<AuthMiddleware> &app) {

  // --- GET /api/events ---
  // Parameter: ?start=YYYY-MM-DD&end=YYYY-MM-DD
  CROW_ROUTE(app, "/api/events")
  ([&](const crow::request &req) {
    const auto &ctx = app.get_context<AuthMiddleware>(req);

    auto start = req.url_params.get("start");
    auto end = req.url_params.get("end");

    if (!start || !end) {
      return crow::response(400, "Missing start/end date params");
    }

    // Events aus der DB laden (nur Gruppen-Events des Users)
    auto events = CakeEvent::getRange(start, end, ctx.currentUser.userId);

    crow::json::wvalue result = crow::json::wvalue::list();
    int i = 0;
    for (const auto &e : events) {
      result[i++] = e.toJson();
    }
    return crow::response(result);
  });

  // --- POST /api/events ---
  // Erwartet: multipart/form-data
  // Felder: date, description (optional), photo (optional File)
  CROW_ROUTE(app, "/api/events")
      .methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
        const auto &ctx = app.get_context<AuthMiddleware>(req);

        // 1. Multipart Nachricht parsen
        crow::multipart::message msg(req);

        QString date;
        QString description;
        QString savedFileName;

        // 2. Parts iterieren und robust nach dem "name" Parameter suchen
        for (const auto &part : msg.parts) {
          const auto &contentDisposition =
              part.headers.find("Content-Disposition");

          if (contentDisposition != part.headers.end()) {
            // Parameter extrahieren (z.B. name="date")
            auto params = contentDisposition->second.params;

            if (params.find("name") != params.end()) {
              std::string name = params["name"];

              if (name == "date") {
                date = QString::fromStdString(part.body);
              } else if (name == "description") {
                description = QString::fromStdString(part.body);
              } else if (name == "photo") {
                // Nur speichern, wenn auch wirklich Daten gesendet wurden
                if (part.body.size() > 0) {
                  // Dateinamen generieren (UUID + .jpg)
                  // In Produktion: Mime-Type prüfen für korrekte Extension
                  QString ext = ".jpg";
                  QString fileName =
                      QUuid::createUuid().toString(QUuid::WithoutBraces) + ext;

                  // Pfad: data/uploads/
                  QString savePath =
                      QDir::current().filePath("data/uploads/" + fileName);

                  // Ordner sicherstellen
                  QDir().mkpath("data/uploads");

                  // Speichern
                  QFile file(savePath);
                  if (file.open(QIODevice::WriteOnly)) {
                    file.write(part.body.data(), part.body.size());
                    file.close();
                    savedFileName = fileName;
                  } else {
                    std::cerr << "Fehler beim Speichern der Datei: "
                              << savePath.toStdString() << std::endl;
                  }
                }
              }
            }
          }
        }

        if (date.isEmpty()) {
          return crow::response(400, "Date required");
        }

        // 3. Event Objekt befüllen
        CakeEvent e;
        e.date = date;
        e.description = description;
        e.photoPath = savedFileName; // Pfad im Model setzen

        // 4. In DB speichern
        if (e.create(ctx.currentUser.userId)) {
          // WICHTIG: JSON zurückgeben, kein Plain Text!
          crow::json::wvalue res;
          res["message"] = "Event created successfully";
          res["id"] = e.id.toStdString();

          return crow::response(201, res);
        } else {
          crow::json::wvalue err;
          err["error"] = "Could not create event. Is user assigned to a group?";
          return crow::response(500, err);
        }
      });
}
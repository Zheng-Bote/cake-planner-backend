/**
 * @file main.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Entry Point
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "crow.h"
#include "database.hpp"
#include "rz_config.hpp"
#include "utils/env_loader.hpp"
#include "utils/seeder.hpp"
#include "utils/app_helpers.hpp"

#include <QCoreApplication>
#include <QString>
#include <QDir>
#include <QDateTime>
#include <thread>
#include <iostream>
#include <string>
#include <csignal>

// Logging (SPDLOG)
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"

// Middleware & Controller Includes
#include "controllers/admin_controller.hpp"
#include "controllers/auth_controller.hpp"
#include "controllers/event_controller.hpp"
#include "controllers/user_controller.hpp"
#include "controllers/system_controller.hpp"
#include "middleware/auth_middleware.hpp"

// SMTP & Models
#include "models/config_model.hpp"
#include "services/smtp_service.hpp"
#include "services/notification_service.hpp"

namespace fs = std::filesystem;

// Global function for safe shutdown (necessary for Signal Handler)
std::function<void()> shutdownHandler;

/**
 * @brief Signal handler callback.
 *
 * Invokes the shutdown handler lambda to safely stop the application.
 *
 * @param signum The signal number (e.g., SIGINT, SIGTERM).
 */
void signalHandler(int signum) {
    // Calls the lambda function in main()
    if(shutdownHandler) {
        shutdownHandler();
    }
}

/**
 * @brief Main entry point of the application.
 *
 * Initializes the environment, logging, database, and services.
 * Configures the Crow web server and starts the Qt event loop.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return Exit code.
 */
int main(int argc, char *argv[]) {
    QCoreApplication qtApp(argc, argv);

    // 1. Load environment
    QString envFilePath = (argc > 1) ? QString::fromUtf8(argv[1]) : QDir::homePath() + "/CakePlanner.env";
    if (!QFile::exists(envFilePath)) {
        std::cerr << "WARNING: .env file not found at: " << envFilePath.toStdString() << std::endl;
    }
    rz::utils::EnvLoader::load(envFilePath.toStdString());

    // 2. Logging Setup
    std::string logDir = rz::utils::EnvLoader::getString("LOG_DIR", "logs");
    std::string logLevelStr = rz::utils::EnvLoader::getString("LOG_LEVEL", "info");

    rz::utils::ensureDirectoryExists(logDir);

#ifdef PROJECT_NAME
    std::string projName = PROJECT_NAME;
#else
    std::string projName = "CakePlanner";
#endif
    std::string logFile = logDir + "/" + projName + ".log";

    try {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFile, 1024 * 1024 * 5, 3);
        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
        auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        spdlog::set_default_logger(logger);

        if (logLevelStr == "debug") spdlog::set_level(spdlog::level::debug);
        else if (logLevelStr == "warn") spdlog::set_level(spdlog::level::warn);
        else if (logLevelStr == "error") spdlog::set_level(spdlog::level::err);
        else spdlog::set_level(spdlog::level::info);

        // --- FIX: Flush on Error AND periodically every 3 seconds ---
        spdlog::flush_on(spdlog::level::err);
        spdlog::flush_every(std::chrono::seconds(3));

        spdlog::info("Logging initialized. Level: {}, File: {}", logLevelStr, logFile);

    } catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    // --- STARTUP LOGS ---
    spdlog::info("Starting {} v{}", std::string(rz::config::PROG_LONGNAME), std::string(rz::config::VERSION));
    spdlog::info("Server Start Time: {}", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString());
    spdlog::info("Using Configuration File: {}", envFilePath.toStdString());

    // 3. Database Setup
    std::string dbFullPath = rz::utils::EnvLoader::getString("DB_DIR", "data/cakeplanner.sqlite");
    spdlog::info("Using Database File: {}", dbFullPath);

    if (!dbFullPath.empty()) {
        std::filesystem::path dbP(dbFullPath);
        if (dbP.has_parent_path()) rz::utils::ensureDirectoryExists(dbP.parent_path().string());
    }

    DatabaseManager::instance().initialize(QString::fromStdString(dbFullPath));
    if (!DatabaseManager::instance().migrate()) {
        spdlog::critical("Abort: Database migration failed.");
        return -1;
    }
    rz::utils::Seeder::ensureAdminExists();

    // 4. Upload Directory
    std::string uploadDir = rz::utils::EnvLoader::getString("UPLOAD_DIR", "public/uploads");
    rz::utils::ensureDirectoryExists(uploadDir);
    spdlog::info("Using Upload Directory: {}", uploadDir);

    // 5. Services Setup
    rz::model::ConfigModel configModel;
    configModel.loadEnv(envFilePath.toStdString());

    rz::service::SmtpService smtpService(configModel, &qtApp);
    rz::service::NotificationService notifyService(&smtpService);

    // 6. Crow App Setup
    crow::App<rz::middleware::AuthMiddleware> app;

    if (logLevelStr == "debug") app.loglevel(crow::LogLevel::Debug);
    else if (logLevelStr == "warn") app.loglevel(crow::LogLevel::Warning);
    else app.loglevel(crow::LogLevel::Info);

    CROW_CATCHALL_ROUTE(app)
    ([&](const crow::request& req, crow::response& res) {
        std::string msg = "An error occurred";
        if (res.code == 404) msg = "Resource not found";
        else if (res.code == 401) msg = "Unauthorized";
        else if (res.code == 500) msg = "Internal Server Error";
        else if (res.code == 400) msg = "Invalid Request";

        res.body = rz::utils::makeErrorJson(res.code, msg).dump();
        res.set_header("Content-Type", "application/json");
        res.end();
    });

    // 7. Register Controllers
    rz::controller::SystemController::registerRoutes(app);
    rz::controller::AuthController authController(&notifyService);
    authController.registerRoutes(app);
    rz::controller::UserController::registerRoutes(app, &notifyService);
    rz::controller::EventController::registerRoutes(app, &notifyService);
    rz::controller::AdminController::registerRoutes(app, &notifyService);

    // 8. Start Server & Signal Handling
    int serverPort = rz::utils::EnvLoader::getInt("CAKE_SERVER_PORT", 8080);
    spdlog::info("Server listening on port: {}", serverPort);

    // Define Shutdown Handler Logic
    shutdownHandler = [&]() {
        spdlog::info("Shutdown Signal received. Stopping Crow...");
        app.stop();     // Stops Crow
        qtApp.quit();   // Stops Qt Event Loop
    };

    // Register signals (Ctrl+C and SIGTERM)
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    std::thread serverThread([&app, serverPort, &qtApp](){
        int serverThreads = rz::utils::EnvLoader::getInt("CAKE_THREADS", 0);
        if (serverThreads > 0) {
            spdlog::info("Using {} threads for server", serverThreads);
            app.port(serverPort).concurrency(serverThreads).run();
        } else {
            const auto processor_count = std::thread::hardware_concurrency();
            spdlog::info("Using {} threads for server", processor_count);
            app.port(serverPort).multithreaded().run();
        }
        // Fallback in case Crow stops by itself (without signal)
        qtApp.quit();
    });

    // Start Qt Event Loop
    int exitCode = qtApp.exec();

    // --- CLEANUP ---
    app.stop(); // Ensure Crow stops
    if (serverThread.joinable()) {
        serverThread.join();
    }

    spdlog::info("Server End Time: {}", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toStdString());
    spdlog::info("Server shutting down with code: {}", exitCode);

    // IMPORTANT: Flush logs immediately now
    spdlog::shutdown();

    return exitCode;
}

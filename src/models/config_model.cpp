/**
 * @file config_model.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Configuration Model implementation
 * @version 0.2.0
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

// WICHTIG: Include auf den neuen Dateinamen (snake_case) angepasst
#include "models/config_model.hpp"
#include <dotenv.h>
#include <QDebug>
#include <cstdlib>

namespace rz {
namespace model {

ConfigModel::ConfigModel() = default;

/**
 * @brief Load environment variables from a file.
 *
 * Uses dotenv-cpp to load the file. Falls back to system environment variables
 * if the file is missing or values are not found in the file.
 *
 * @param path The path to the .env file.
 */
void ConfigModel::loadEnv(const std::string& path) {
    try {
        dotenv::init(path.c_str());
    } catch (const std::exception& e) {
        qWarning() << "Could not load .env file from" << QString::fromStdString(path)
                   << ":" << e.what();
        // Warnung reicht, wir versuchen Fallback auf System-Env
    }

    // Helper Lambda für sauberen Env-Zugriff mit Default-Werten
    auto getEnv = [](const char* key, const char* defaultVal = "") -> QString {
        const char* val = std::getenv(key);
        return val ? QString::fromUtf8(val) : QString::fromUtf8(defaultVal);
    };

    m_smtpServer = getEnv("SMTP_SERVER");
    m_smtpPort = getEnv("SMTP_PORT", "587").toInt();
    m_smtpUsername = getEnv("SMTP_USERNAME");
    m_smtpPassword = getEnv("SMTP_PASSWORD");
    m_smtpFrom = getEnv("SMTP_FROM");

    QString startTls = getEnv("SMTP_STARTTLS", "true");
    m_smtpStartTls = (startTls.compare("true", Qt::CaseInsensitive) == 0);

    m_watchDir = getEnv("WATCH_DIR", ".");

    qInfo() << "Loaded Configuration for SMTP Server:" << m_smtpServer;
}

QString ConfigModel::getSmtpServer() const { return m_smtpServer; }
QString ConfigModel::getSmtpUsername() const { return m_smtpUsername; }
QString ConfigModel::getSmtpPassword() const { return m_smtpPassword; }
QString ConfigModel::getSmtpFrom() const { return m_smtpFrom; }
int ConfigModel::getSmtpPort() const { return m_smtpPort; }
bool ConfigModel::getSmtpStartTls() const { return m_smtpStartTls; }
QString ConfigModel::getWatchDir() const { return m_watchDir; }

} // namespace model
} // namespace rz

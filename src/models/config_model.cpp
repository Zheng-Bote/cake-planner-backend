/**
 * @file config_model.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Configuration Model implementation
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

// IMPORTANT: Include adapted to new filename (snake_case)
#include "models/config_model.hpp"
#include <dotenv.h>
#include <QDebug>
#include <cstdlib>

namespace rz {
namespace model {

/**
 * @brief Constructs the ConfigModel.
 */
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
        // Warning is sufficient, we try fallback to system env
    }

    // Helper lambda for clean env access with default values
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

/**
 * @brief Gets the SMTP server address.
 * @return The SMTP server.
 */
QString ConfigModel::getSmtpServer() const { return m_smtpServer; }

/**
 * @brief Gets the SMTP username.
 * @return The SMTP username.
 */
QString ConfigModel::getSmtpUsername() const { return m_smtpUsername; }

/**
 * @brief Gets the SMTP password.
 * @return The SMTP password.
 */
QString ConfigModel::getSmtpPassword() const { return m_smtpPassword; }

/**
 * @brief Gets the SMTP Sender email address.
 * @return The SMTP from address.
 */
QString ConfigModel::getSmtpFrom() const { return m_smtpFrom; }

/**
 * @brief Gets the SMTP port.
 * @return The SMTP port.
 */
int ConfigModel::getSmtpPort() const { return m_smtpPort; }

/**
 * @brief Checks if STARTTLS is enabled for SMTP.
 * @return True if STARTTLS is enabled, false otherwise.
 */
bool ConfigModel::getSmtpStartTls() const { return m_smtpStartTls; }

/**
 * @brief Gets the directory being watched for changes.
 * @return The watch directory.
 */
QString ConfigModel::getWatchDir() const { return m_watchDir; }

} // namespace model
} // namespace rz

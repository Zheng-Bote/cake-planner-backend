/**
 * SPDX-FileComment: Configuration Model implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file config_model.cpp
 * @brief Configuration Model implementation
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

// IMPORTANT: Include adapted to new filename (snake_case)
#include "models/config_model.hpp"
#include "utils/env_loader.hpp"
#include <QDebug>

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief model namespace.
 */
namespace model {

/**
 * @brief Constructs the ConfigModel.
 */
ConfigModel::ConfigModel() = default;

/**
 * @brief Load environment variables from a file.
 *
 * Uses EnvLoader to load the file. Falls back to system environment variables
 * if the file is missing or values are not found in the file.
 *
 * @param path The path to the .env file.
 */
void ConfigModel::loadEnv(const std::string& path) {
    // Note: EnvLoader::load is already called in main.cpp, 
    // but we can call it again if path is different, 
    // or just rely on EnvLoader::get which checks the environment.
    rz::utils::EnvLoader::load(path);

    m_smtpServer = rz::utils::EnvLoader::get("SMTP_SERVER");
    m_smtpPort = rz::utils::EnvLoader::getInt("SMTP_PORT", 587);
    m_smtpUsername = rz::utils::EnvLoader::get("SMTP_USERNAME");
    m_smtpPassword = rz::utils::EnvLoader::get("SMTP_PASSWORD");
    m_smtpFrom = rz::utils::EnvLoader::get("SMTP_FROM");

    QString startTls = rz::utils::EnvLoader::get("SMTP_STARTTLS", "true");
    m_smtpStartTls = (startTls.compare("true", Qt::CaseInsensitive) == 0);

    m_watchDir = rz::utils::EnvLoader::get("WATCH_DIR", ".");

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

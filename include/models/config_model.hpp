/**
 * @file ConfigModel.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Configuration Model to load env vars
 * @version 0.1.0
 * @date 2026-01-02
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include <QString>
#include <string>

namespace rz {
namespace model {

/**
 * @brief The ConfigModel class handles loading and providing access to configuration settings.
 *
 * It reads environment variables from a .env file or the system environment
 * to configure SMTP settings and the directory to watch.
 */
class ConfigModel {
public:
  /**
   * @brief Construct a new Config Model object.
   */
  ConfigModel();

  /**
   * @brief Load environment variables from a file.
   *
   * @param path The path to the .env file.
   */
  void loadEnv(const std::string& path);

  /**
   * @brief Get the SMTP Server address.
   * @return QString The SMTP server hostname.
   */
  QString getSmtpServer() const;

  /**
   * @brief Get the SMTP Port.
   * @return int The SMTP port number (default: 587).
   */
  int getSmtpPort() const;

  /**
   * @brief Get the SMTP Username.
   * @return QString The username for SMTP authentication.
   */
  QString getSmtpUsername() const;

  /**
   * @brief Get the SMTP Password.
   * @return QString The password for SMTP authentication.
   */
  QString getSmtpPassword() const;

  /**
   * @brief Get the Sender Email Address.
   * @return QString The email address to use in the 'From' field.
   */
  QString getSmtpFrom() const;

  /**
   * @brief Check if STARTTLS is enabled.
   * @return true if STARTTLS is enabled (default), false otherwise.
   */
  bool getSmtpStartTls() const;

  /**
   * @brief Get the Directory path to watch for files.
   * @return QString The absolute or relative path to the watch directory.
   */
  QString getWatchDir() const;

private:
  QString m_smtpServer;
  int m_smtpPort = 587;
  QString m_smtpUsername;
  QString m_smtpPassword;
  QString m_smtpFrom;
  bool m_smtpStartTls = true;
  QString m_watchDir;
};

} // namespace model
} // namespace rz

/**
 * SPDX-FileComment: SMTP Service Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file smtp_service.hpp
 * @brief SMTP Service Header
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once

#include "models/config_model.hpp"
#include <QObject>
#include <QString>
#include <QMetaType>

// Forward declaration
namespace SimpleMail { class ServerReply; }

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief service namespace.
 */
namespace service {

/**
 * @brief SmtpService class.
 */
class SmtpService : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Function implementation.
     */
    explicit SmtpService(const model::ConfigModel& config, QObject* parent = nullptr);
    ~SmtpService();

    // This method is THREAD-SAFE and can be called from Crow controllers
    void sendEmailAsync(const QString& to, const QString& subject, const QString& body,
                        const QByteArray& attachmentData = QByteArray(), const QString& attachmentName = QString());

private slots:
    // This slot runs in the main thread
    void doSendEmail(const QString& to, const QString& subject, const QString& body,
                     const QByteArray& attachmentData = QByteArray(), const QString& attachmentName = QString());

private:
    model::ConfigModel m_config;
};

} // namespace service
} // namespace rz

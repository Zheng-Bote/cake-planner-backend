/**
 * SPDX-FileComment: SMTP Service Header
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file smtp_service.hpp
 * @brief SMTP Service Header
 * @version 1.4.0
 * @date 2026-04-12
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
#include <QQueue>
#include <memory>

// Forward declaration
namespace SimpleMail { 
    class ServerReply; 
    class Server;
    class MimeMessage;
}

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief service namespace.
 */
namespace service {

/**
 * @brief Helper struct for queued emails.
 */
struct EmailTask {
    QString to;
    QString subject;
    QString body;
    QByteArray attachmentData;
    QString attachmentName;

    EmailTask() = default;
    EmailTask(const QString& t, const QString& s, const QString& b, const QByteArray& ad, const QString& an)
        : to(t), subject(s), body(b), attachmentData(ad), attachmentName(an) {}
};

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
    void processNextEmail();

private:

    model::ConfigModel m_config;
    QQueue<EmailTask> m_emailQueue;
    SimpleMail::Server* m_server = nullptr; // Persistent server
    int m_activeConnections = 0;
    const int MAX_CONCURRENT_EMAILS = 1;
};

} // namespace service
} // namespace rz

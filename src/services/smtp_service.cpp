/**
 * SPDX-FileComment: SMTP Service Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file smtp_service.cpp
 * @brief SMTP Service Implementation
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#include "services/smtp_service.hpp"

// CORRECTION: Direct includes (without "SimpleMail/" prefix), as FetchContent is used
#include "server.h"
#include "mimemessage.h"
#include "mimetext.h"
#include "mimeattachment.h"
#include "emailaddress.h"
#include "serverreply.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMetaObject>

#include "spdlog/spdlog.h"

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief service namespace.
 */
namespace service {

/**
 * @brief Constructs the SmtpService.
 *
 * @param config Configuration model containing SMTP settings.
 * @param parent Optional parent QObject.
 */
SmtpService::SmtpService(const model::ConfigModel& config, QObject* parent)
    : QObject(parent), m_config(config) {
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<QByteArray>("QByteArray");
}

SmtpService::~SmtpService() = default;

/**
 * @brief Sends an email asynchronously using Qt's event loop.
 *
 * @param to Recipient email address.
 * @param subject Email subject.
 * @param body Email body content.
 * @param attachmentData Optional attachment data.
 * @param attachmentName Optional attachment filename.
 */
void SmtpService::sendEmailAsync(const QString& to, const QString& subject, const QString& body,
                                 const QByteArray& attachmentData, const QString& attachmentName) {
    QMetaObject::invokeMethod(this, "doSendEmail", Qt::QueuedConnection,
                              Q_ARG(QString, to),
                              Q_ARG(QString, subject),
                              Q_ARG(QString, body),
                              Q_ARG(QByteArray, attachmentData),
                              Q_ARG(QString, attachmentName));
}

/**
 * @brief Internal slot to perform the actual email sending.
 *
 * This method is invoked via QMetaObject::invokeMethod to run on the correct thread/loop context.
 *
 * @param to Recipient email address.
 * @param subject Email subject.
 * @param body Email body content.
 * @param attachmentData Optional attachment data.
 * @param attachmentName Optional attachment filename.
 */
void SmtpService::doSendEmail(const QString& to, const QString& subject, const QString& body,
                               const QByteArray& attachmentData, const QString& attachmentName) {
    qInfo() << "[SMTP] Preparing email to:" << to;

    auto server = new SimpleMail::Server(this);
    server->setHost(m_config.getSmtpServer());
    server->setPort(m_config.getSmtpPort());
    server->setUsername(m_config.getSmtpUsername());
    server->setPassword(m_config.getSmtpPassword());

    if (m_config.getSmtpStartTls()) {
        server->setConnectionType(SimpleMail::Server::TlsConnection);
    } else {
        server->setConnectionType(SimpleMail::Server::TcpConnection);
    }

    SimpleMail::MimeMessage message;
    message.setSender(SimpleMail::EmailAddress(m_config.getSmtpFrom(), "CakePlanner Bot"));
    message.addTo(SimpleMail::EmailAddress(to));
    message.setSubject(subject);

    auto textPart = std::make_shared<SimpleMail::MimeText>();
    textPart->setText(body);
    message.addPart(textPart);

    if (!attachmentData.isEmpty() && !attachmentName.isEmpty()) {
        auto attachmentPart = std::make_shared<SimpleMail::MimeAttachment>(attachmentData, attachmentName);
        message.addPart(attachmentPart);
    }

    SimpleMail::ServerReply* reply = server->sendMail(message);

    connect(reply, &SimpleMail::ServerReply::finished, [reply, server, to]() {
        if (reply->error()) {
            //qWarning() << "[SMTP] Failed to send to" << to << ":" << reply->responseText();
            spdlog::warn("[SMTP] Failed to send to {}", to.toStdString());
        } else {
            //qInfo() << "[SMTP] Sent successfully to" << to;
            spdlog::info("[SMTP] Sent successfully to {}", to.toStdString());
        }
        reply->deleteLater();
        server->deleteLater();
    });
}

} // namespace service
} // namespace rz

/**
 * @file smtp_service.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief SMTP Service Implementation
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "services/smtp_service.hpp"

// CORRECTION: Direct includes (without "SimpleMail/" prefix), as FetchContent is used
#include "server.h"
#include "mimemessage.h"
#include "mimetext.h"
#include "emailaddress.h"
#include "serverreply.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMetaObject>

#include "spdlog/spdlog.h"

namespace rz {
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
}

SmtpService::~SmtpService() = default;

/**
 * @brief Sends an email asynchronously using Qt's event loop.
 *
 * @param to Recipient email address.
 * @param subject Email subject.
 * @param body Email body content.
 */
void SmtpService::sendEmailAsync(const QString& to, const QString& subject, const QString& body) {
    QMetaObject::invokeMethod(this, "doSendEmail", Qt::QueuedConnection,
                              Q_ARG(QString, to),
                              Q_ARG(QString, subject),
                              Q_ARG(QString, body));
}

/**
 * @brief Internal slot to perform the actual email sending.
 *
 * This method is invoked via QMetaObject::invokeMethod to run on the correct thread/loop context.
 *
 * @param to Recipient email address.
 * @param subject Email subject.
 * @param body Email body content.
 */
void SmtpService::doSendEmail(const QString& to, const QString& subject, const QString& body) {
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

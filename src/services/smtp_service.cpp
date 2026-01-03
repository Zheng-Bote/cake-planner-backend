/**
 * @file smtp_service.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief SMTP Service Implementation
 * @version 0.2.1
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "services/smtp_service.hpp"

// KORREKTUR: Direkte Includes (ohne "SimpleMail/" Prefix), da FetchContent genutzt wird
#include "server.h"
#include "mimemessage.h"
#include "mimetext.h"
#include "emailaddress.h"
#include "serverreply.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMetaObject>

namespace rz {
namespace service {

SmtpService::SmtpService(const model::ConfigModel& config, QObject* parent)
    : QObject(parent), m_config(config) {
    qRegisterMetaType<QString>("QString");
}

SmtpService::~SmtpService() = default;

void SmtpService::sendEmailAsync(const QString& to, const QString& subject, const QString& body) {
    QMetaObject::invokeMethod(this, "doSendEmail", Qt::QueuedConnection,
                              Q_ARG(QString, to),
                              Q_ARG(QString, subject),
                              Q_ARG(QString, body));
}

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
            qWarning() << "[SMTP] Failed to send to" << to << ":" << reply->responseText();
        } else {
            qInfo() << "[SMTP] Sent successfully to" << to;
        }
        reply->deleteLater();
        server->deleteLater();
    });
}

} // namespace service
} // namespace rz

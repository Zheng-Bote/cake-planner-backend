/**
 * SPDX-FileComment: SMTP Service Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file smtp_service.cpp
 * @brief SMTP Service Implementation
 * @version 1.4.0
 * @date 2026-04-12
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#include "services/smtp_service.hpp"

// SimpleMail includes
#include "server.h"
#include "mimemessage.h"
#include "mimetext.h"
#include "mimeattachment.h"
#include "emailaddress.h"
#include "serverreply.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMetaObject>
#include <QPointer>
#include <QTimer>

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
 * @brief Internal helper class to handle a single email sending job.
 */
class SmtpSendJob : public QObject {
    Q_OBJECT
public:
    SmtpSendJob(const EmailTask& task, const model::ConfigModel& config, QObject* parent = nullptr)
        : QObject(parent), m_task(task), m_config(config) {}

    void run() {
        QString host = m_config.getSmtpServer();
        QString from = m_config.getSmtpFrom();

        if (host.isEmpty() || from.isEmpty()) {
            spdlog::error("[SMTP-Job] Invalid configuration.");
            emit finished();
            return;
        }

        // Create everything on the heap with this job as parent
        m_server = new SimpleMail::Server(this);
        m_server->setHost(host);
        m_server->setPort(m_config.getSmtpPort());
        m_server->setUsername(m_config.getSmtpUsername());
        m_server->setPassword(m_config.getSmtpPassword());

        if (m_config.getSmtpStartTls()) {
            m_server->setConnectionType(SimpleMail::Server::TlsConnection);
        } else {
            m_server->setConnectionType(SimpleMail::Server::TcpConnection);
        }

        // We MUST use std::shared_ptr for message and parts as required by SimpleMail 3.x
        auto message = std::make_shared<SimpleMail::MimeMessage>();
        message->setSender(SimpleMail::EmailAddress(from, "CakePlanner Bot"));
        message->addTo(SimpleMail::EmailAddress(m_task.to));
        message->setSubject(m_task.subject);

        auto textPart = std::make_shared<SimpleMail::MimeText>();
        textPart->setText(m_task.body);
        message->addPart(textPart);

        if (!m_task.attachmentData.isEmpty() && !m_task.attachmentName.isEmpty()) {
            auto attachmentPart = std::make_shared<SimpleMail::MimeAttachment>(m_task.attachmentData, m_task.attachmentName);
            message->addPart(attachmentPart);
        }

        m_keepAliveMessage = message;

        spdlog::debug("[SMTP-Job] Calling sendMail for {}", m_task.to.toStdString());
        SimpleMail::ServerReply* reply = m_server->sendMail(*message);
        
        if (!reply) {
            spdlog::error("[SMTP-Job] server->sendMail returned nullptr!");
            emit finished();
            return;
        }

        connect(reply, &SimpleMail::ServerReply::finished, this, [this, reply]() {
            if (reply->error()) {
                spdlog::warn("[SMTP-Job] Failed: {}", reply->responseText().toStdString());
            } else {
                spdlog::info("[SMTP-Job] Success");
            }
            reply->deleteLater();
            emit finished();
        });
    }

signals:
    void finished();

private:
    EmailTask m_task;
    model::ConfigModel m_config; // COPY of config for safety
    std::shared_ptr<SimpleMail::MimeMessage> m_keepAliveMessage;
    SimpleMail::Server* m_server = nullptr;
};

// --- SmtpService ---

SmtpService::SmtpService(const model::ConfigModel& config, QObject* parent)
    : QObject(parent), m_config(config) {
    qRegisterMetaType<QString>("QString");
    qRegisterMetaType<QByteArray>("QByteArray");
}

SmtpService::~SmtpService() = default;

void SmtpService::sendEmailAsync(const QString& to, const QString& subject, const QString& body,
                                 const QByteArray& attachmentData, const QString& attachmentName) {
    QMetaObject::invokeMethod(this, "doSendEmail", Qt::QueuedConnection,
                              Q_ARG(QString, to),
                              Q_ARG(QString, subject),
                              Q_ARG(QString, body),
                              Q_ARG(QByteArray, attachmentData),
                              Q_ARG(QString, attachmentName));
}

void SmtpService::doSendEmail(const QString& to, const QString& subject, const QString& body,
                               const QByteArray& attachmentData, const QString& attachmentName) {
    
    m_emailQueue.enqueue(EmailTask(to, subject, body, attachmentData, attachmentName));
    
    if (m_activeConnections < MAX_CONCURRENT_EMAILS) {
        processNextEmail();
    }
}

void SmtpService::processNextEmail() {
    if (m_emailQueue.isEmpty() || m_activeConnections >= MAX_CONCURRENT_EMAILS) {
        return;
    }

    EmailTask task = m_emailQueue.dequeue();
    m_activeConnections++;

    spdlog::info("[SMTP] Processing email to {}. Connections: {}", task.to.toStdString(), m_activeConnections);

    SmtpSendJob* job = new SmtpSendJob(task, m_config, this);
    connect(job, &SmtpSendJob::finished, this, [this, job]() {
        job->deleteLater();
        m_activeConnections--;
        QMetaObject::invokeMethod(this, "processNextEmail", Qt::QueuedConnection);
    });

    job->run();
}

} // namespace service
} // namespace rz

#include "smtp_service.moc"

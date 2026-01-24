/**
 * @file smtp_service.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief SMTP Service Header
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "models/config_model.hpp"
#include <QObject>
#include <QString>
#include <QMetaType>

// Forward declaration
namespace SimpleMail { class ServerReply; }

namespace rz {
namespace service {

class SmtpService : public QObject {
    Q_OBJECT
public:
    explicit SmtpService(const model::ConfigModel& config, QObject* parent = nullptr);
    ~SmtpService();

    // This method is THREAD-SAFE and can be called from Crow controllers
    void sendEmailAsync(const QString& to, const QString& subject, const QString& body);

private slots:
    // This slot runs in the main thread
    void doSendEmail(const QString& to, const QString& subject, const QString& body);

private:
    model::ConfigModel m_config;
};

} // namespace service
} // namespace rz

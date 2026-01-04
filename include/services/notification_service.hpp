/**
 * @file notification_service.hpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Notification Service
 * @version 0.2.1
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "smtp_service.hpp"
#include <QString>
#include <vector>

namespace rz {
namespace service {

class NotificationService {
public:
    explicit NotificationService(SmtpService* smtp);

    // Info an Admins bei neuer Registrierung
    void notifyAdminsNewUser(const QString& newUserName, const QString& newUserEmail);

    // Info an Gruppe bei neuem Kuchen
    void notifyGroupNewEvent(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn);

    // Account Status Mails
    void notifyAccountActivated(const QString& email, const QString& name, const QString& lang);
    void notifyAccountDeactivated(const QString& email, const QString& name, const QString& lang);
    void notifyAccountDeleted(const QString& email, const QString& name, const QString& lang);
    void notifyPasswordChanged(const QString& email, const QString& name, const QString& lang);

private:
    SmtpService* m_smtp;

    // Hilfsmethode um alle globalen Admins aus der DB zu holen
    std::vector<QString> getGlobalAdminEmails();
};

} // namespace service
} // namespace rz

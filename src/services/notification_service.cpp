/**
 * @file notification_service.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Notification Service Implementation
 * @version 0.1.0
 * @date 2026-01-03
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "services/notification_service.hpp"
#include "database.hpp"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

namespace rz {
namespace service {

NotificationService::NotificationService(SmtpService* smtp)
    : m_smtp(smtp) {}

std::vector<QString> NotificationService::getGlobalAdminEmails() {
    std::vector<QString> emails;
    auto db = DatabaseManager::instance().getDatabase();
    QSqlQuery query(db);
    // Hole alle User mit is_admin = 1
    query.prepare("SELECT email FROM users WHERE is_admin = 1 AND is_active = 1");
    if (query.exec()) {
        while (query.next()) {
            emails.push_back(query.value("email").toString());
        }
    }
    return emails;
}

void NotificationService::notifyAdminsNewUser(const QString& newUserName, const QString& newUserEmail) {
    auto admins = getGlobalAdminEmails();
    if (admins.empty()) {
        qWarning() << "No admins found to notify.";
        return;
    }

    QString subject = "CakePlanner: Neuer User registriert";
    QString body = QString("Ein neuer User hat sich registriert:\n\nName: %1\nEmail: %2\n\nBitte prüfen und ggf. Gruppe zuweisen.")
                   .arg(newUserName, newUserEmail);

    for (const auto& adminEmail : admins) {
        m_smtp->sendEmailAsync(adminEmail, subject, body);
    }
}

void NotificationService::notifyGroupNewEvent(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn) {
    // Deutsch
    if (!recipientsDe.empty()) {
        QString subject = QString("Neuer Kuchen in %1!").arg(groupName);
        QString body = QString("Hallo,\n\n%1 bringt am %2 einen Kuchen mit!\n\nYummy!").arg(bakerName, date);
        for (const auto& mail : recipientsDe) m_smtp->sendEmailAsync(mail, subject, body);
    }

    // Englisch
    if (!recipientsEn.empty()) {
        QString subject = QString("New Cake in %1!").arg(groupName);
        QString body = QString("Hello,\n\n%1 is bringing a cake on %2!\n\nYummy!").arg(bakerName, date);
        for (const auto& mail : recipientsEn) m_smtp->sendEmailAsync(mail, subject, body);
    }
}

} // namespace service
} // namespace rz

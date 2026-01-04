/**
 * @file notification_service.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Notification Service Implementation
 * @version 0.2.1
 * @date 2026-01-04
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

void NotificationService::notifyAccountActivated(const QString& email, const QString& name, const QString& lang) {
    QString subject;
    QString body;

    if (lang == "de") {
        subject = "Dein Account wurde aktiviert - Cake Planner";
        body = QString("Hallo %1,\n\n"
                       "Gute Nachrichten! Dein Account wurde von einem Administrator freigeschaltet.\n"
                       "Du kannst dich jetzt einloggen und am Kuchen-Planen teilnehmen.\n\n"
                       "Viel Spaß,\nDein Cake Planner Bot").arg(name);
    } else {
        subject = "Account Activated - Cake Planner";
        body = QString("Hello %1,\n\n"
                       "Good news! Your account has been activated by an administrator.\n"
                       "You can now log in and join the cake planning.\n\n"
                       "Enjoy,\nYour Cake Planner Bot").arg(name);
    }

    m_smtp->sendEmailAsync(email, subject, body);
}

void NotificationService::notifyAccountDeactivated(const QString& email, const QString& name, const QString& lang) {
    QString subject;
    QString body;

    if (lang == "de") {
        subject = "Dein Account wurde deaktiviert";
        body = QString("Hallo %1,\n\n"
                       "Dein Account wurde vorübergehend von einem Administrator deaktiviert.\n"
                       "Du kannst dich aktuell nicht einloggen. Bitte wende dich an den Admin, falls das ein Fehler ist.\n\n"
                       "Grüße,\nDein Cake Planner Bot").arg(name);
    } else {
        subject = "Account Deactivated";
        body = QString("Hello %1,\n\n"
                       "Your account has been temporarily deactivated by an administrator.\n"
                       "You cannot log in at the moment. Please contact the admin if this is a mistake.\n\n"
                       "Regards,\nYour Cake Planner Bot").arg(name);
    }

    m_smtp->sendEmailAsync(email, subject, body);
}

void NotificationService::notifyAccountDeleted(const QString& email, const QString& name, const QString& lang) {
    QString subject;
    QString body;

    if (lang == "de") {
        subject = "Dein Account wurde gelöscht";
        body = QString("Hallo %1,\n\n"
                       "Dein Account wurde endgültig gelöscht und deine persönlichen Daten wurden anonymisiert.\n"
                       "Schade, dass du gehst!\n\n"
                       "Alles Gute,\nDein Cake Planner Bot").arg(name);
    } else {
        subject = "Account Deleted";
        body = QString("Hello %1,\n\n"
                       "Your account has been permanently deleted and your personal data has been anonymized.\n"
                       "We are sorry to see you go!\n\n"
                       "Best wishes,\nYour Cake Planner Bot").arg(name);
    }

    m_smtp->sendEmailAsync(email, subject, body);
}

void NotificationService::notifyPasswordChanged(const QString& email, const QString& name, const QString& lang) {
    QString subject;
    QString body;

    if (lang == "de") {
        subject = "Sicherheitswarnung: Dein Passwort wurde geändert";
        body = QString("Hallo %1,\n\n"
                       "Dein Passwort für den Cake Planner wurde gerade geändert.\n"
                       "Falls du das warst, kannst du diese E-Mail ignorieren.\n\n"
                       "FALLS NICHT: Bitte kontaktiere sofort den Administrator!\n\n"
                       "Viele Grüße,\nDein Cake Planner Bot").arg(name);
    } else {
        subject = "Security Alert: Your password has been changed";
        body = QString("Hello %1,\n\n"
                       "Your password for the Cake Planner was just changed.\n"
                       "If this was you, you can ignore this email.\n\n"
                       "IF NOT: Please contact the administrator immediately!\n\n"
                       "Best regards,\nYour Cake Planner Bot").arg(name);
    }

    m_smtp->sendEmailAsync(email, subject, body);
}

} // namespace service
} // namespace rz

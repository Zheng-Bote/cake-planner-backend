/**
 * SPDX-FileComment: Notification Service Implementation
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file notification_service.cpp
 * @brief Notification Service Implementation
 * @version 1.2.0
 * @date 2026-04-11
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#include "services/notification_service.hpp"
#include "database.hpp"
#include "models/user_model.hpp"
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
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
 * @brief Constructs the NotificationService.
 *
 * @param smtp Pointer to the SMTP service used for sending emails.
 */
NotificationService::NotificationService(SmtpService* smtp)
    : m_smtp(smtp) {}

/**
 * @brief Sends an email to all members of a group with automatic translation.
 *
 * This method fetches all active users in the specified group, groups them by their
 * preferred email language, and sends a translated version of the message to each language group.
 *
 * @param groupId The ID of the group.
 * @param text The original message to translate and send.
 */
void NotificationService::sendGroupEmail(const QString& groupId, const QString& text) {
    auto users = User::getAll(groupId);
    if (users.empty()) {
        spdlog::warn("sendGroupEmail: No users found for group {}", groupId.toStdString());
        return;
    }

    // Group users by email language
    std::map<QString, std::vector<QString>> languageGroups;
    for (const auto& user : users) {
        if (user.is_active) {
            languageGroups[user.emailLanguage].push_back(user.email);
        }
    }

    // Process each language
    for (auto const& [lang, emails] : languageGroups) {
        QString translated = translateText(text, lang);
        QString subject = (lang == "de") ? "Nachricht vom Administrator" : "Message from Administrator";

        for (const auto& email : emails) {
            m_smtp->sendEmailAsync(email, subject, translated);
        }
    }
}

/**
 * @brief Translates text using an external translation API.
 *
 * Sends a POST request to a local translation service and parses the JSON response.
 *
 * @param text The text to translate.
 * @param targetLang The target language code (e.g., "de", "en").
 * @return The translated text, or the original text if translation fails.
 */
QString NotificationService::translateText(const QString& text, const QString& targetLang) {
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://localhost:18080/api/v1/prompt"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    QString prompt = QString("translate to %1: %2").arg(targetLang, text);
    body["prompt"] = prompt;

    QJsonDocument doc(body);
    QNetworkReply* reply = manager.post(request, doc.toJson());

    // Use a local event loop to wait for the reply (synchronous feel for the backend thread)
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject responseObj = responseDoc.object();
        if (responseObj.contains("response") && responseObj["status"].toString() == "success") {
            QString translated = responseObj["response"].toString();
            reply->deleteLater();
            return translated;
        }
    }

    spdlog::error("Translation failed for lang {}: {}", targetLang.toStdString(), reply->errorString().toStdString());
    reply->deleteLater();
    return text; // Fallback to original text
}

/**
 * @brief Retrieves the email addresses of all global administrators.
 *
 * @return A vector of admin email addresses.
 */
std::vector<QString> NotificationService::getGlobalAdminEmails() {
    std::vector<QString> emails;
    auto db = DatabaseManager::instance().getDatabase();
    /**
     * @brief Function implementation.
     */
    QSqlQuery query(db);
    // Get all users with is_admin = 1
    query.prepare("SELECT email FROM users WHERE is_admin = 1 AND is_active = 1");
    if (query.exec()) {
        while (query.next()) {
            emails.push_back(query.value("email").toString());
        }
    }
    return emails;
}

/**
 * @brief Notifies admins about a new user registration.
 *
 * @param newUserName The name of the new user.
 * @param newUserEmail The email of the new user.
 */
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

/**
 * @brief Notifies group members about a new event (e.g., someone bringing cake).
 *
 * Sends emails in German or English based on the recipients' preferences.
 *
 * @param groupName The name of the group.
 * @param bakerName The name of the user bringing the cake.
 * @param date The date of the event.
 * @param recipientsDe List of email addresses for German notifications.
 * @param recipientsEn List of email addresses for English notifications.
 */
void NotificationService::notifyGroupNewEvent(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn, const QByteArray& icsData) {
    QString icsName = "cake_event.ics";

    // German
    if (!recipientsDe.empty()) {
        QString subject = QString("Neuer Kuchen in %1!").arg(groupName);
        QString body = QString("Hallo,\n\n%1 bringt am %2 einen Kuchen mit!\n\nYummy!").arg(bakerName, date);
        for (const auto& mail : recipientsDe) {
            if (!icsData.isEmpty()) m_smtp->sendEmailAsync(mail, subject, body, icsData, icsName);
            else m_smtp->sendEmailAsync(mail, subject, body);
        }
    }

    // English
    if (!recipientsEn.empty()) {
        QString subject = QString("New Cake in %1!").arg(groupName);
        QString body = QString("Hello,\n\n%1 is bringing a cake on %2!\n\nYummy!").arg(bakerName, date);
        for (const auto& mail : recipientsEn) {
            if (!icsData.isEmpty()) m_smtp->sendEmailAsync(mail, subject, body, icsData, icsName);
            else m_smtp->sendEmailAsync(mail, subject, body);
        }
    }
}

void NotificationService::notifyGroupEventDeleted(const QString& groupName, const QString& bakerName, const QString& date,
                                                  const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn) {

    // German
    if (!recipientsDe.empty()) {
        QString subjectDe = QString("Kuchen-Absage: %1").arg(date);
        QString bodyDe = QString("Hallo,\n\nleider wurde der Kuchen-Termin am %1 von %2 in der Gruppe '%3' abgesagt.\n\nViele Grüße,\nDein CakePlanner")
                         .arg(date, bakerName, groupName);
        for (const auto& mail : recipientsDe) m_smtp->sendEmailAsync(mail, subjectDe, bodyDe);
    }
    // English
    if (!recipientsEn.empty()) {
        QString subjectEn = QString("Cake Cancellation: %1").arg(date);
        QString bodyEn = QString("Hello,\n\nunfortunately, the cake event on %1 by %2 in group '%3' has been cancelled.\n\nBest regards,\nYour CakePlanner")
                         .arg(date, bakerName, groupName);
        for (const auto& mail : recipientsEn) m_smtp->sendEmailAsync(mail, subjectEn, bodyEn);
    }

}

/**
 * @brief Notifies a user that their account has been activated.
 *
 * @param email The user's email address.
 * @param name The user's name.
 * @param lang The user's preferred language ("de" or "en").
 */
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

/**
 * @brief Notifies a user that their account has been deactivated.
 *
 * @param email The user's email address.
 * @param name The user's name.
 * @param lang The user's preferred language ("de" or "en").
 */
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

/**
 * @brief Notifies a user that their account has been deleted.
 *
 * @param email The user's email address.
 * @param name The user's name.
 * @param lang The user's preferred language ("de" or "en").
 */
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

/**
 * @brief Notifies a user that their password has been changed.
 *
 * Serves as a security alert.
 *
 * @param email The user's email address.
 * @param name The user's name.
 * @param lang The user's preferred language ("de" or "en").
 */
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

/**
 * @brief Sends a temporary password to the user.
 *
 * @param email The user's email address.
 * @param name The user's name.
 * @param tempPassword The temporary password.
 * @param lang The user's preferred language.
 */
void NotificationService::notifyForgotPassword(const QString& email, const QString& name, const QString& tempPassword, const QString& lang) {
    QString subject;
    QString body;

    if (lang == "de") {
        subject = "Dein temporäres Passwort - Cake Planner";
        body = QString("Hallo %1,\n\n"
                       "Du hast ein neues Passwort angefordert.\n"
                       "Dein temporäres Passwort lautet:\n\n"
                       "%2\n\n"
                       "Dieses Passwort ist 24 Stunden gültig.\n"
                       "Bitte ändere dein Passwort sofort nach der Anmeldung.\n\n"
                       "Viele Grüße,\nDein Cake Planner Bot").arg(name, tempPassword);
    } else {
        subject = "Your temporary password - Cake Planner";
        body = QString("Hello %1,\n\n"
                       "You requested a new password.\n"
                       "Your temporary password is:\n\n"
                       "%2\n\n"
                       "This password is valid for 24 hours.\n"
                       "Please change your password immediately after logging in.\n\n"
                       "Best regards,\nYour Cake Planner Bot").arg(name, tempPassword);
    }

    m_smtp->sendEmailAsync(email, subject, body);
}

} // namespace service
} // namespace rz

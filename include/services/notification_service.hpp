#pragma once

#include "smtp_service.hpp"
#include <QString>
#include <vector>

namespace rz {
namespace service {

class NotificationService {
public:
    explicit NotificationService(SmtpService* smtp);

    // Punkt 4: Info an Admins bei neuer Registrierung
    void notifyAdminsNewUser(const QString& newUserName, const QString& newUserEmail);

    // Punkt 2b (Später): Info an Gruppe bei neuem Kuchen
    void notifyGroupNewEvent(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn);

private:
    SmtpService* m_smtp;

    // Hilfsmethode um alle globalen Admins aus der DB zu holen
    std::vector<QString> getGlobalAdminEmails();
};

} // namespace service
} // namespace rz

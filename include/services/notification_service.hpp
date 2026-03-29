/**
 * SPDX-FileComment: Notification Service
 * SPDX-FileType: SOURCE
 * SPDX-FileContributor: ZHENG Robert
 * SPDX-FileCopyrightText: 2026 ZHENG Robert
 * SPDX-License-Identifier: MIT
 *
 * @file notification_service.hpp
 * @brief Notification Service
 * @version 0.15.0
 * @date 2026-01-24
 *
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * @license MIT
 */

#pragma once

#include "smtp_service.hpp"
#include <QString>
#include <vector>

/**
 * @brief rz namespace.
 */
namespace rz {
/**
 * @brief service namespace.
 */
namespace service {

/**
 * @brief NotificationService class.
 */
class NotificationService {
public:
    /**
     * @brief Function implementation.
     */
    explicit NotificationService(SmtpService* smtp);

    // Notify admins about new registration
    void notifyAdminsNewUser(const QString& newUserName, const QString& newUserEmail);

    // Notify group about new cake
    void notifyGroupNewEvent(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn, const QByteArray& icsData = QByteArray());
    // Notify group about deleted cake
    void notifyGroupEventDeleted(const QString& groupName, const QString& bakerName, const QString& date, const std::vector<QString>& recipientsDe, const std::vector<QString>& recipientsEn);

    // Account Status Emails
    void notifyAccountActivated(const QString& email, const QString& name, const QString& lang);
    /**
     * @brief Function implementation.
     */
    void notifyAccountDeactivated(const QString& email, const QString& name, const QString& lang);
    /**
     * @brief Function implementation.
     */
    void notifyAccountDeleted(const QString& email, const QString& name, const QString& lang);
    /**
     * @brief Function implementation.
     */
    void notifyPasswordChanged(const QString& email, const QString& name, const QString& lang);
    /**
     * @brief Function implementation.
     */
    void notifyForgotPassword(const QString& email, const QString& name, const QString& tempPassword, const QString& lang);

private:
    SmtpService* m_smtp;

    // Helper method to fetch all global admins from DB
    std::vector<QString> getGlobalAdminEmails();
};

} // namespace service
} // namespace rz

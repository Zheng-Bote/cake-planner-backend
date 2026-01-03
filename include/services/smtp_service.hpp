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

    // Diese Methode ist THREAD-SAFE und kann aus Crow-Controllern aufgerufen werden
    void sendEmailAsync(const QString& to, const QString& subject, const QString& body);

private slots:
    // Dieser Slot läuft im Main-Thread
    void doSendEmail(const QString& to, const QString& subject, const QString& body);

private:
    model::ConfigModel m_config;
};

} // namespace service
} // namespace rz

#include "utils/env_loader.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

// Speichert die geladenen Werte temporär im Prozess-Environment
void EnvLoader::load(const QString &filename) {
  // Suchstrategie:
  // 1. Neben dem Executable (AppImage Ordner)
  // 2. Im aktuellen Arbeitsverzeichnis (Docker Workdir)
  QStringList searchPaths = {QCoreApplication::applicationDirPath(),
                             QDir::currentPath()};

  QString foundPath;
  for (const auto &path : searchPaths) {
    QString fullPath = QDir(path).filePath(filename);
    if (QFile::exists(fullPath)) {
      foundPath = fullPath;
      break;
    }
  }

  if (foundPath.isEmpty()) {
    qWarning() << "Warnung: .env Datei nicht gefunden:" << filename
               << "- Verwende Standardwerte.";
    return;
  }

  qInfo() << "Lade Konfiguration aus:" << foundPath;
  QFile file(foundPath);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.isEmpty() || line.startsWith('#'))
        continue;

      int splitIndex = line.indexOf('=');
      if (splitIndex > 0) {
        QString key = line.left(splitIndex).trimmed();
        QString value = line.mid(splitIndex + 1).trimmed();

        // Anführungszeichen entfernen falls vorhanden
        if (value.startsWith('"') && value.endsWith('"')) {
          value = value.mid(1, value.length() - 2);
        }

        // In das Prozess-Environment setzen (für qEnvironmentVariable)
        qputenv(key.toUtf8(), value.toUtf8());
      }
    }
  }
}

QString EnvLoader::get(const QString &key, const QString &defaultValue) {
  QString val = qEnvironmentVariable(key.toUtf8());
  return val.isEmpty() ? defaultValue : val;
}

int EnvLoader::getInt(const QString &key, int defaultValue) {
  QString val = qEnvironmentVariable(key.toUtf8());
  bool ok;
  int num = val.toInt(&ok);
  return ok ? num : defaultValue;
}
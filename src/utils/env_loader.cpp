/**
 * @file env_loader.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Environment Loader Implementation
 * @version 0.1.4
 * @date 2026-01-22
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */

#include "utils/env_loader.hpp"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

namespace rz {
namespace utils {

/**
 * @brief Loads environment variables from a file.
 *
 * Searches for the .env file in the application directory and the current working directory.
 * If found, it loads the variables into the process environment.
 *
 * @param filenameStd The name of the file to load (default: .env).
 */
void EnvLoader::load(const std::string &filenameStd) {
  QString filename = QString::fromStdString(filenameStd);

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
    qWarning() << "Warning: .env file not found:" << filename
               << "- Using default/system values.";
    return;
  }

  qInfo() << "Loading configuration from:" << foundPath;
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

        if (value.startsWith('"') && value.endsWith('"')) {
          value = value.mid(1, value.length() - 2);
        }
        qputenv(key.toUtf8(), value.toUtf8());
      }
    }
  }
}

/**
 * @brief Retrieves an environment variable as a QString.
 *
 * @param keyStd The key of the environment variable.
 * @param defaultValueStd The default value if the key is not found.
 * @return The value of the environment variable or the default value.
 */
QString EnvLoader::get(const std::string &keyStd, const std::string &defaultValueStd) {
  QString key = QString::fromStdString(keyStd);
  QString val = qEnvironmentVariable(key.toUtf8());

  return val.isEmpty() ? QString::fromStdString(defaultValueStd) : val;
}

/**
 * @brief Retrieves an environment variable as a std::string.
 *
 * @param keyStd The key of the environment variable.
 * @param defaultValueStd The default value if the key is not found.
 * @return The value of the environment variable or the default value.
 */
std::string EnvLoader::getString(const std::string &keyStd, const std::string &defaultValueStd) {
  QString key = QString::fromStdString(keyStd);
  QString val = qEnvironmentVariable(key.toUtf8());

  return val.isEmpty() ? defaultValueStd : val.toStdString();
}

/**
 * @brief Retrieves an environment variable as an integer.
 *
 * @param keyStd The key of the environment variable.
 * @param defaultValue The default value if the key is not found or not a valid number.
 * @return The integer value of the environment variable or the default value.
 */
int EnvLoader::getInt(const std::string &keyStd, int defaultValue) {
  QString key = QString::fromStdString(keyStd);
  QString val = qEnvironmentVariable(key.toUtf8());

  bool ok;
  int num = val.toInt(&ok);
  return ok ? num : defaultValue;
}

} // namespace utils
} // namespace rz

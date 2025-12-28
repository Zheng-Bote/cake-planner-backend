#pragma once
#include <QString>

class EnvLoader {
public:
  static void load(const QString &filename = "CakePlanner.env");
  static QString get(const QString &key, const QString &defaultValue = "");
  static int getInt(const QString &key, int defaultValue);
};
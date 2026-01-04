/**
 * @file image_processor.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Image Processor Implementation
 * @version 0.1.2
 * @date 2026-01-04
 *
 * @copyright Copyright (c) 2025 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */


#include "utils/image_processor.hpp"
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// Zielgrößen
const std::vector<int> ImageProcessor::TARGET_WIDTHS = {480, 800, 1280};

void ImageProcessor::generateWebPVersions(const QString& sourcePath) {
    QImage img(sourcePath);
    if (img.isNull()) {
        qCritical() << "ImageProcessor: Failed to load image. Check path and Qt Plugins (JPEG support)!" << sourcePath;
        // Tipp: Auf Linux muss oft 'libqt6-imageformats' installiert sein
        return;
    }

    QFileInfo fileInfo(sourcePath);
    QDir dir = fileInfo.dir();
    QString baseName = fileInfo.completeBaseName();

    qDebug() << "ImageProcessor: Processing" << baseName << "in" << dir.absolutePath();

    for (int width : TARGET_WIDTHS) {
        QString webpFilename = QString("%1__%2.webp").arg(baseName).arg(width);
        QString targetPath = dir.filePath(webpFilename);

        bool success = false;

        // Fall 1: Original ist kleiner als Ziel -> Nur Konvertieren, nicht hochskalieren
        if (img.width() <= width) {
            // Wir speichern das Original als WebP (Qualität 90)
            success = img.save(targetPath, "WEBP", 90);
        }
        // Fall 2: Skalieren
        else {
            QImage scaled = img.scaledToWidth(width, Qt::SmoothTransformation);
            success = scaled.save(targetPath, "WEBP", 85);
        }

        if (success) {
            // qDebug() << "Created:" << targetPath;
        } else {
            qCritical() << "ImageProcessor: Failed to save WebP to" << targetPath
                        << "- check if WEBP plugin is available!";
        }
    }
}

void ImageProcessor::deleteAllVersions(const QString& sourcePath) {
    QFileInfo fileInfo(sourcePath);
    QDir dir = fileInfo.dir();
    QString baseName = fileInfo.completeBaseName();

    // Löscht Original
    QFile::remove(sourcePath);

    // Löscht WebP Versionen
    for (int width : TARGET_WIDTHS) {
        QString webpName = QString("%1__%2.webp").arg(baseName).arg(width);
        if (dir.exists(webpName)) {
            dir.remove(webpName);
        }
    }
}

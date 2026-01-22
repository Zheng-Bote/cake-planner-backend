/**
 * @file image_processor.cpp
 * @author ZHENG Robert (robert@hase-zheng.net)
 * @brief Image Processor Implementation
 * @version 0.1.3
 * @date 2026-01-22
 *
 * @copyright Copyright (c) 2026 ZHENG Robert
 *
 * SPDX-License-Identifier: MIT
 */


#include "utils/image_processor.hpp"
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

// Target sizes
const std::vector<int> ImageProcessor::TARGET_WIDTHS = {480, 800, 1280};

/**
 * @brief Generates WebP versions of an image in multiple resolutions.
 *
 * Creates images with widths defined in TARGET_WIDTHS (480, 800, 1280).
 * If the source image is smaller than a target width, it is just converted to WebP without upscaling.
 *
 * @param sourcePath The file path of the source image.
 */
void ImageProcessor::generateWebPVersions(const QString& sourcePath) {
    QImage img(sourcePath);
    if (img.isNull()) {
        qCritical() << "ImageProcessor: Failed to load image. Check path and Qt Plugins (JPEG support)!" << sourcePath;
        // Tip: On Linux 'libqt6-imageformats' often needs to be installed
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

        // Case 1: Original is smaller than target -> Convert only, do not upscale
        if (img.width() <= width) {
            // Save original as WebP (Quality 90)
            success = img.save(targetPath, "WEBP", 90);
        }
        // Case 2: Scale
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

/**
 * @brief Deletes the original image and all generated WebP versions.
 *
 * @param sourcePath The file path of the original image (used to derive version filenames).
 */
void ImageProcessor::deleteAllVersions(const QString& sourcePath) {
    QFileInfo fileInfo(sourcePath);
    QDir dir = fileInfo.dir();
    QString baseName = fileInfo.completeBaseName();

    // Deletes original
    QFile::remove(sourcePath);

    // Deletes WebP versions
    for (int width : TARGET_WIDTHS) {
        QString webpName = QString("%1__%2.webp").arg(baseName).arg(width);
        if (dir.exists(webpName)) {
            dir.remove(webpName);
        }
    }
}

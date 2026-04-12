# image_processor.cpp

## File Header Information

| Field | Value |
| :--- | :--- |
| **SPDX Comment** | Image Processor Implementation |
| **SPDX Type** | SOURCE |
| **Contributor** | ZHENG Robert |
| **License ID** | MIT |
| **File** | `image_processor.cpp` |
| **Description** | Image Processor Implementation |
| **Version** | 0.15.0 |
| **Date** | 2026-01-24 |
| **Author** | ZHENG Robert (robert@hase-zheng.net) |
| **Copyright** | Copyright (c) 2026 ZHENG Robert |
| **License** | MIT |

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**

- [API Documentation](#api-documentation)
  - [`void ImageProcessor::generateWebPVersions(const QString& sourcePath) `](#void-imageprocessorgeneratewebpversionsconst-qstring-sourcepath-)
  - [`QImage img(sourcePath)`](#qimage-imgsourcepath)
  - [`QFileInfo fileInfo(sourcePath)`](#qfileinfo-fileinfosourcepath)
  - [`void ImageProcessor::deleteAllVersions(const QString& sourcePath) `](#void-imageprocessordeleteallversionsconst-qstring-sourcepath-)
  - [`QFileInfo fileInfo(sourcePath)`](#qfileinfo-fileinfosourcepath-1)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## API Documentation

### `void ImageProcessor::generateWebPVersions(const QString& sourcePath) `

> Generates WebP versions of an image in multiple resolutions.

| Parameter | Description |
| --- | --- |
| `sourcePath` | The file path of the source image. |

---

### `QImage img(sourcePath)`

> Function implementation.

---

### `QFileInfo fileInfo(sourcePath)`

> Function implementation.

---

### `void ImageProcessor::deleteAllVersions(const QString& sourcePath) `

> Deletes the original image and all generated WebP versions.

| Parameter | Description |
| --- | --- |
| `sourcePath` | The file path of the original image (used to derive version filenames). |

---

### `QFileInfo fileInfo(sourcePath)`

> Function implementation.

---


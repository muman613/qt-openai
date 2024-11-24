#include "ImageDownloader.h"
#include <QFile>
#include <QDir>
#include <QDebug>

ImageDownloader::ImageDownloader(const QString &url, QObject *parent)
    : QObject(parent),
      manager(new QNetworkAccessManager(this)),
      currentReply(nullptr), // No active reply initially
      imageUrl(url),
      downloadPath(QDir::currentPath()), // Default to current directory
      downloadFileName("downloaded_image.jpg"), // Default file name
      timeoutDuration(30000) { // Default timeout: 30 seconds
    timeoutTimer.setSingleShot(true); // Ensure the timer triggers only once
    connect(&timeoutTimer, &QTimer::timeout, this, &ImageDownloader::onTimeout);
}

void ImageDownloader::setDownloadPath(const QString &path) {
    QMutexLocker locker(&mutex); // Ensure thread-safe access
    downloadPath = path;
}

void ImageDownloader::setDownloadFileName(const QString &fileName) {
    QMutexLocker locker(&mutex); // Ensure thread-safe access
    downloadFileName = fileName;
}

void ImageDownloader::setTimeout(int milliseconds) {
    QMutexLocker locker(&mutex); // Ensure thread-safe access
    timeoutDuration = milliseconds;
}

void ImageDownloader::start() {
    QMutexLocker locker(&mutex); // Protect shared resources during the start

    QUrl url(imageUrl);
    if (!url.isValid() || url.scheme().isEmpty()) {
        emit downloadFailed(QString("Invalid URL: %1").arg(imageUrl));
        return;
    }

    if (currentReply) {
        emit downloadFailed("Download already in progress");
        return;
    }

    QNetworkRequest request(url);
    currentReply = manager->get(request);

    connect(currentReply, &QNetworkReply::finished, this, &ImageDownloader::onDownloadFinished);
    connect(currentReply, &QNetworkReply::errorOccurred, this, &ImageDownloader::onReplyError);
    connect(currentReply, &QNetworkReply::downloadProgress, this, [](qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "Download Progress:" << bytesReceived << "/" << bytesTotal;
    });

    timeoutTimer.start(timeoutDuration); // Start the timeout timer
}

void ImageDownloader::onReplyError(QNetworkReply::NetworkError code) {
    QMutexLocker locker(&mutex);

    QString errorReason = currentReply ? currentReply->errorString() : "Unknown error";
    emit downloadFailed(QString("Network error (%1): %2").arg(code).arg(errorReason));

    timeoutTimer.stop(); // Stop the timer on error
    cleanUpReply(); // Clean up resources
}

void ImageDownloader::onDownloadFinished() {
    QMutexLocker locker(&mutex);

    timeoutTimer.stop(); // Stop the timeout timer

    if (!currentReply || currentReply->error() != QNetworkReply::NoError) {
        // Error case is handled by the errorOccurred signal
        cleanUpReply();
        return;
    }

    QByteArray data = currentReply->readAll();
    cleanUpReply(); // Release network resources early

    // Ensure the download path exists
    QDir dir(downloadPath);
    if (!dir.exists() && !dir.mkpath(".")) {
        emit downloadFailed(QString("Failed to create directory: %1").arg(downloadPath));
        return;
    }

    // Build full file path
    QString fullFilePath = dir.filePath(downloadFileName);

    QFile file(fullFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
        emit downloadComplete(fullFilePath);
    } else {
        emit downloadFailed(QString("Failed to save the file: %1").arg(fullFilePath));
    }
}

void ImageDownloader::onTimeout() {
    QMutexLocker locker(&mutex);

    if (currentReply) {
        currentReply->abort(); // Abort the ongoing request
    }

    emit downloadFailed("Download timed out");
    cleanUpReply(); // Clean up resources
}

void ImageDownloader::cleanUpReply() {
    if (currentReply) {
        currentReply->deleteLater(); // Schedule for cleanup
        currentReply = nullptr;
    }
}

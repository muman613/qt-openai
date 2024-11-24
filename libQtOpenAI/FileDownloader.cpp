#include "FileDownloader.h"
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QEventLoop>


FileDownloader::FileDownloader(const QString &url, QObject *parent)
    : QObject(parent),
      manager(nullptr), // Will be created in run()
      currentReply(nullptr),
      fileUrl(url),
      downloadPath(QDir::currentPath()),
      downloadFileName("downloaded_file"),
      timeoutDuration(30000) { // Default timeout: 30 seconds
}

void FileDownloader::setDownloadPath(const QString &path) {
    downloadPath = path;
}

void FileDownloader::setDownloadFileName(const QString &fileName) {
    downloadFileName = fileName;
}

void FileDownloader::setTimeout(int milliseconds) {
    timeoutDuration = milliseconds;
}

void FileDownloader::run() {
    manager = new QNetworkAccessManager(); // Create in the thread
    connect(manager, &QNetworkAccessManager::finished, this, &FileDownloader::onDownloadFinished);

    QUrl url(fileUrl);
    if (!url.isValid() || url.scheme().isEmpty()) {
        emit downloadFailed(QString("Invalid URL: %1").arg(fileUrl));
        return;
    }

    QNetworkRequest request(url);
    currentReply = manager->get(request);

    connect(currentReply, &QNetworkReply::errorOccurred, this, &FileDownloader::onReplyError);
    connect(currentReply, &QNetworkReply::downloadProgress, this, [](qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "Download Progress:" << bytesReceived << "/" << bytesTotal;
    });

    timeoutTimer.setSingleShot(true);
    connect(&timeoutTimer, &QTimer::timeout, this, &FileDownloader::onTimeout);
    timeoutTimer.start(timeoutDuration);

    // Start the event loop for asynchronous operations
    QEventLoop loop;
    connect(this, &FileDownloader::downloadComplete, &loop, &QEventLoop::quit);
    connect(this, &FileDownloader::downloadFailed, &loop, &QEventLoop::quit);
    loop.exec();

    cleanUpReply();
}

void FileDownloader::onReplyError(QNetworkReply::NetworkError code) {
    QString errorReason = currentReply ? currentReply->errorString() : "Unknown error";
    emit downloadFailed(QString("Network error (%1): %2").arg(code).arg(errorReason));
    timeoutTimer.stop();
    cleanUpReply();
}

void FileDownloader::onDownloadFinished() {
    timeoutTimer.stop();

    if (!currentReply || currentReply->error() != QNetworkReply::NoError) {
        // Error case is handled in onReplyError
        cleanUpReply();
        return;
    }

    QByteArray data = currentReply->readAll();
    cleanUpReply();

    QDir dir(downloadPath);
    if (!dir.exists() && !dir.mkpath(".")) {
        emit downloadFailed(QString("Failed to create directory: %1").arg(downloadPath));
        return;
    }

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

void FileDownloader::onTimeout() {
    if (currentReply) {
        currentReply->abort();
    }
    emit downloadFailed("Download timed out");
    cleanUpReply();
}

void FileDownloader::cleanUpReply() {
    if (currentReply) {
        currentReply->deleteLater();
        currentReply = nullptr;
    }
    delete manager; // Clean up manager after the task is done
    manager = nullptr;
}

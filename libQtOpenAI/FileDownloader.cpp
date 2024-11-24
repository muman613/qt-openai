#include "FileDownloader.h"
#include <QFile>
#include <QDir>
#include <QDate>
#include <QDebug>

FileDownloader::FileDownloader(const QString &url, QObject *parent)
    : QObject(parent),
      manager(new QNetworkAccessManager(this)),
      currentReply(nullptr),
      timeoutTimer(new QTimer(this)),
      fileUrl(url),
      downloadPath(QDir::currentPath()),
      downloadFileName("downloaded_file"),
      fileExtension(""),
      namingMode(SimpleName),
      sequenceNumber(0),
      timeoutDuration(30000) { // Default timeout: 30 seconds
    timeoutTimer->setSingleShot(true);
    connect(timeoutTimer, &QTimer::timeout, this, &FileDownloader::onTimeout);
    connect(manager, &QNetworkAccessManager::finished, this, &FileDownloader::onDownloadFinished);
}

void FileDownloader::setDownloadPath(const QString &path) {
    downloadPath = path;
}

void FileDownloader::setDownloadFileName(const QString &fileName) {
    downloadFileName = fileName;
}

void FileDownloader::setFileExtension(const QString &extension) {
    fileExtension = extension;
}

void FileDownloader::setFileNamingMode(FileDownloader::NamingMode mode) {
    namingMode = mode;
}

void FileDownloader::setSequenceStart(int start) {
    sequenceNumber = start;
}

void FileDownloader::setTimeout(int milliseconds) {
    timeoutDuration = milliseconds;
}

void FileDownloader::start() {
    QUrl url(fileUrl);
    if (!url.isValid() || url.scheme().isEmpty()) {
        emit downloadFailed(QString("Invalid URL: %1").arg(fileUrl));
        return;
    }

    QNetworkRequest request(url);
    currentReply = manager->get(request);

    connect(currentReply, &QNetworkReply::errorOccurred, this, &FileDownloader::onReplyError);
    timeoutTimer->start(timeoutDuration);
}

QString FileDownloader::generateFileName() const {
    QString baseName = downloadFileName;
    QString suffix;

    switch (namingMode) {
    case SimpleName:
        return downloadPath + "/" + baseName;

    case WithExtension:
        return downloadPath + "/" + baseName + "." + fileExtension;

    case SequenceNumber:
        suffix = QString("_%1").arg(sequenceNumber, 3, 10, QChar('0'));
        return downloadPath + "/" + baseName + suffix + "." + fileExtension;

    case WithDatestamp:
        suffix = "_" + QDate::currentDate().toString("yyyyMMdd");
        return downloadPath + "/" + baseName + suffix + "." + fileExtension;
    }

    return downloadPath + "/" + baseName; // Fallback
}

void FileDownloader::onDownloadFinished() {
    timeoutTimer->stop();

    if (!currentReply || currentReply->error() != QNetworkReply::NoError) {
        cleanUpReply();
        return;
    }

    QByteArray data = currentReply->readAll();
    cleanUpReply();

    QString fullFilePath = generateFileName();

    QFile file(fullFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
        emit downloadComplete(fullFilePath);
    } else {
        emit downloadFailed(QString("Failed to save the file: %1").arg(fullFilePath));
    }

    if (namingMode == SequenceNumber) {
        ++sequenceNumber;
    }
}

void FileDownloader::onReplyError(QNetworkReply::NetworkError) {
    emit downloadFailed(currentReply ? currentReply->errorString() : "Unknown error");
    timeoutTimer->stop();
    cleanUpReply();
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
}

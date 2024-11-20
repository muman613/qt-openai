#include "ImageDownloader.h"
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

ImageDownloader::ImageDownloader(const QString &url, QObject *parent)
    : QObject(parent),
      manager(new QNetworkAccessManager(this)),
      imageUrl(url),
      downloadPath(QDir::currentPath()), // Default to current directory
      downloadFileName("downloaded_image.jpg") // Default file name
{}

void ImageDownloader::setDownloadPath(const QString &path) {
    downloadPath = path;
}

void ImageDownloader::setDownloadFileName(const QString &fileName) {
    downloadFileName = fileName;
}

void ImageDownloader::start() {
    QUrl url(imageUrl);
    if (!url.isValid() || url.scheme().isEmpty()) {
        emit downloadFailed(QString("Invalid URL: %1").arg(imageUrl));
        return;
    }

    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onDownloadFinished(reply); });
    connect(reply, &QNetworkReply::downloadProgress, this, [](qint64 bytesReceived, qint64 bytesTotal) {
        qDebug() << "Download Progress:" << bytesReceived << "/" << bytesTotal;
    });
}

void ImageDownloader::onDownloadFinished(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        emit downloadFailed(reply->errorString());
        return;
    }

    QByteArray data = reply->readAll();

    // Ensure the download path exists
    QDir dir(downloadPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            emit downloadFailed(QString("Failed to create directory: %1").arg(downloadPath));
            return;
        }
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

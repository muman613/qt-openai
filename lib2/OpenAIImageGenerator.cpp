
#include "OpenAIImageGenerator.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include "ImageDownloader.h"

OpenAIImageGenerator::OpenAIImageGenerator(QObject *parent)
    : OpenAIBaseClient(parent) {
    setApiUrl("https://api.openai.com/v1/images/generations");

 // Connect network manager's finished signal to the handleImageGenerationReply slot
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &OpenAIImageGenerator::handleImageGenerationReply);
}

void OpenAIImageGenerator::generateImage(const QString &prompt) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key is not set.");
        return;
    }

    QJsonObject requestObject;
    requestObject["prompt"] = prompt;
    requestObject["n"] = 1;
    requestObject["size"] = "512x512";

    QJsonDocument jsonDoc(requestObject);
    QByteArray requestData = jsonDoc.toJson();

    QNetworkRequest request; // (QUrl(m_apiUrl));
    request.setUrl(m_apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    m_networkManager->post(request, requestData);
}

// void OpenAIImageGenerator::handleImageGenerationReply(QNetworkReply *reply) {
//     qDebug() << Q_FUNC_INFO;

//     if (reply->error() == QNetworkReply::NoError) {
//         QByteArray responseData = reply->readAll();
//         QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
//         QString imageUrl = jsonResponse["data"].toArray()[0].toObject()["url"].toString();

//         qDebug() << "URL :" << imageUrl;

//         ImageDownloader download(imageUrl);

//         download.start();

//         // emit imageGenerated("downloaded_image.jpg");
//     } else {
//         emit errorOccurred(reply->errorString());
//     }
//     reply->deleteLater();
// }

void OpenAIImageGenerator::handleImageGenerationReply(QNetworkReply *reply) {
    qDebug() << Q_FUNC_INFO;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QString imageUrl = jsonResponse["data"].toArray()[0].toObject()["url"].toString();

        qDebug() << "URL :" << imageUrl;

        // Create and configure the ImageDownloader
        ImageDownloader *download = new ImageDownloader(imageUrl, this);
        download->setDownloadFileName("generated_image.png");  // Optional: Set custom file name
        download->setDownloadPath(QDir::currentPath());        // Optional: Set custom download path

        // Connect signals from ImageDownloader to OpenAIImageGenerator
        connect(download, &ImageDownloader::downloadComplete, this, [this, download](const QString &filePath) {
            qDebug() << "Image successfully downloaded to:" << filePath;
            emit imageGenerated(filePath);  // Emit imageGenerated signal
            download->deleteLater();        // Clean up the downloader instance
        });

        connect(download, &ImageDownloader::downloadFailed, this, [this, download](const QString &reason) {
            qCritical() << "Image download failed:" << reason;
            emit errorOccurred(reason);    // Emit errorOccurred signal
            download->deleteLater();       // Clean up the downloader instance
        });

        // Start the download
        download->start();

    } else {
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
}

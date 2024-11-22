#include "OpenAIImageGenerator.h"
#include "ImageDownloader.h"
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

OpenAIImageGenerator::OpenAIImageGenerator(QObject *parent)
    : OpenAIBaseClient(parent), m_downloadPath(QDir::currentPath()), // Default path: current directory
      m_outputFileName("generated_image.jpg"), m_model("dall-e-3") { // Default file name
    setApiUrl("https://api.openai.com/v1/images/generations");

    connect(m_networkManager, &QNetworkAccessManager::finished, this, &OpenAIImageGenerator::handleImageGenerationReply);
}

void OpenAIImageGenerator::setDownloadPath(const QString &path) { m_downloadPath = path; }

QString OpenAIImageGenerator::downloadPath() const { return m_downloadPath; }

void OpenAIImageGenerator::setOutputFileName(const QString &fileName) { m_outputFileName = fileName; }

QString OpenAIImageGenerator::outputFileName() const { return m_outputFileName; }

void OpenAIImageGenerator::generateImage(const QString &prompt) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key is not set.");
        return;
    }

    QJsonObject requestObject;
    requestObject["prompt"] = prompt;
    requestObject["n"] = 1;
    requestObject["size"] = "1024x1024";
    requestObject["model"] = m_model;

    QJsonDocument jsonDoc(requestObject);
    QByteArray requestData = jsonDoc.toJson();

    QNetworkRequest request;
    request.setUrl(m_apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    m_networkManager->post(request, requestData);
}

void OpenAIImageGenerator::handleImageGenerationReply(QNetworkReply *reply) {
    qDebug() << Q_FUNC_INFO;

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QString imageUrl = jsonResponse["data"].toArray()[0].toObject()["url"].toString();

        qDebug() << "URL:" << imageUrl;

        // Create and configure the ImageDownloader
        ImageDownloader *download = new ImageDownloader(imageUrl, this);
        download->setDownloadFileName(m_outputFileName); // Use configured file name
        download->setDownloadPath(m_downloadPath);       // Use configured path

        // Connect signals from ImageDownloader to OpenAIImageGenerator
        connect(download, &ImageDownloader::downloadComplete, this, [this, download](const QString &filePath) {
            qDebug() << "Image successfully downloaded to:" << filePath;
            emit imageGenerated(filePath); // Emit imageGenerated signal
            download->deleteLater();       // Clean up the downloader instance
        });

        connect(download, &ImageDownloader::downloadFailed, this, [this, download](const QString &reason) {
            qCritical() << "Image download failed:" << reason;
            emit errorOccurred(reason); // Emit errorOccurred signal
            download->deleteLater();    // Clean up the downloader instance
        });

        // Start the download
        download->start();

    } else {
        emit errorOccurred(reply->errorString());
    }

    reply->deleteLater();
}

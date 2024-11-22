
#include "OpenAITTSClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

OpenAITTSClient::OpenAITTSClient(QObject *parent)
    : OpenAIBaseClient(parent) {
    setApiUrl("https://api.openai.com/v1/audio/speech");
    setModel("tts-1");
    // Connect the finished signal of QNetworkAccessManager to handleTTSReply
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &OpenAITTSClient::handleTTSReply);
}

void OpenAITTSClient::generateSpeech(const QString &text) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key is not set.");
        return;
    }

    QJsonObject requestObject;
    requestObject["input"] = text;
    requestObject["model"] = m_model;
    requestObject["voice"] = "alloy";

    QJsonDocument jsonDoc(requestObject);
    QByteArray requestData = jsonDoc.toJson();

    QNetworkRequest request(m_apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    m_networkManager->post(request, requestData);
}

void OpenAITTSClient::handleTTSReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray audioData = reply->readAll();
        QString filePath = "synthesized_audio.mp3";
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(audioData);
            file.close();
            emit audioGenerated(filePath);
        } else {
            emit errorOccurred("Failed to save the audio file.");
        }
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

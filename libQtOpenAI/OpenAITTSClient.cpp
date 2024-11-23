#include "OpenAITTSClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QProcessEnvironment>
#include <QDir>

OpenAITTSClient::OpenAITTSClient(QObject *parent)
    : OpenAIBaseClient(parent),
      m_downloadPath(QDir::currentPath()), // Default path: current directory
      m_fileName("synthesized_audio.mp3") { // Default file name
    setApiUrl("https://api.openai.com/v1/audio/speech");
    setModel("tts-1");
    setVoice("alloy");

        // Retrieve the API key from the environment variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = env.value("OPENAI_API_KEY");

    if (apiKey.isEmpty()) {
        qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
    }

    // Set default API key for the image generator
    setApiKey(apiKey); // Replace with your OpenAI API key

    // Connect the finished signal of QNetworkAccessManager to handleTTSReply
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &OpenAITTSClient::handleTTSReply);
}

void OpenAITTSClient::setDownloadPath(const QString &path) {
    m_downloadPath = path;
}

QString OpenAITTSClient::downloadPath() const {
    return m_downloadPath;
}

void OpenAITTSClient::setFileName(const QString &fileName) {
    if (fileName.endsWith(".mp3", Qt::CaseInsensitive)) {
        m_fileName = fileName;
    } else {
        m_fileName = fileName + ".mp3"; // Ensure the .mp3 extension
    }
}

QString OpenAITTSClient::fileName() const {
    return m_fileName;
}

void OpenAITTSClient::generateSpeech(const QString &text) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key is not set.");
        return;
    }

    QJsonObject requestObject;
    requestObject["input"] = text;
    requestObject["model"] = m_model;
    requestObject["voice"] = m_voice;

    QJsonDocument jsonDoc(requestObject);
    QByteArray requestData = jsonDoc.toJson();

    QNetworkRequest request;
    request.setUrl(m_apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    m_networkManager->post(request, requestData);
}

void OpenAITTSClient::handleTTSReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray audioData = reply->readAll();

        // Construct the full path for the audio file
        QString filePath = QDir(m_downloadPath).filePath(m_fileName);

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(audioData);
            file.close();
            emit audioGenerated(filePath); // Emit the completion signal
        } else {
            emit errorOccurred("Failed to save the audio file to " + filePath);
        }
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

void OpenAITTSClient::setVoice(const QString &v) {
    m_voice = v;
}

QString OpenAITTSClient::voice() const {
    return m_voice;
}

QStringList OpenAITTSClient::voices() const {
    static QStringList voiceList = {
        "alloy", "echo", "fable", "onyx", "nova", "shimmer"
    };

    return voiceList;
}
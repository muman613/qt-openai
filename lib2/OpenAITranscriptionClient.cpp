
#include "OpenAITranscriptionClient.h"
#include <QHttpMultiPart>
#include <QFile>
#include <QMimeDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>

OpenAITranscriptionClient::OpenAITranscriptionClient(QObject *parent)
    : OpenAIBaseClient(parent), m_language("") {
    setApiUrl("https://api.openai.com/v1/audio/transcriptions");
    setModel("whisper-1");
}

void OpenAITranscriptionClient::setLanguage(const QString &language) {
    m_language = language;
}

void OpenAITranscriptionClient::transcribeAudio(const QString &filePath) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key is not set.");
        return;
    }

    QFile *audioFile = new QFile(filePath);
    if (!audioFile->exists()) {
        emit errorOccurred("Audio file does not exist.");
        return;
    }
    if (!audioFile->open(QIODevice::ReadOnly)) {
        emit errorOccurred("Failed to open the audio file.");
        delete audioFile;
        return;
    }

    QString mimeType = QMimeDatabase().mimeTypeForFile(filePath).name();
    if (mimeType.isEmpty()) {
        emit errorOccurred("Unsupported file type.");
        delete audioFile;
        return;
    }

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant(mimeType));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name="file"; filename="" + audioFile->fileName() + """));
    filePart.setBodyDevice(audioFile);
    audioFile->setParent(multiPart);

    QHttpPart modelPart;
    modelPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name="model""));
    modelPart.setBody(m_model.toUtf8());

    multiPart->append(filePart);
    multiPart->append(modelPart);

    if (!m_language.isEmpty()) {
        QHttpPart languagePart;
        languagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name="language""));
        languagePart.setBody(m_language.toUtf8());
        multiPart->append(languagePart);
    }

    QNetworkRequest request(QUrl(m_apiUrl));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    m_networkManager->post(request, multiPart);
    multiPart->setParent(m_networkManager);
}

void OpenAITranscriptionClient::handleTranscriptionReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QString transcription = jsonResponse.object().value("text").toString();
        emit transcriptionCompleted(transcription);
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

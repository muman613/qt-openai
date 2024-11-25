
#include "OpenAIAPIClient.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>

OpenAIAPIClient::OpenAIAPIClient(QObject *parent) : OpenAIBaseClient(parent) {
    setApiUrl("https://api.openai.com/v1/chat/completions");
    setModel("gpt-4o");

    // Connect the finished signal of QNetworkAccessManager to handleNetworkReply
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &OpenAIAPIClient::handleNetworkReply);
}

void OpenAIAPIClient::sendMessage(const QString &message) {
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("API key is not set.");
        return;
    }

    QJsonObject requestObject;
    requestObject["model"] = m_model;
    requestObject["messages"] = QJsonArray{QJsonObject{{"role", "user"}, {"content", message}}};

    QJsonDocument jsonDoc(requestObject);
    QByteArray requestData = jsonDoc.toJson();

    QNetworkRequest request; // (QUrl(m_apiUrl));
    request.setUrl(m_apiUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());

    m_networkManager->post(request, requestData);
}

void OpenAIAPIClient::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QString result = jsonResponse["choices"].toArray()[0].toObject()["message"].toObject()["content"].toString();
        emit responseReceived(result);
    } else {
        emit errorOccurred(reply->errorString());
    }
    reply->deleteLater();
}

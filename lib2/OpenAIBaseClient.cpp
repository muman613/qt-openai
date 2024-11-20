
#include "OpenAIBaseClient.h"

OpenAIBaseClient::OpenAIBaseClient(QObject *parent)
    : QObject(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_model("gpt-4") {}

void OpenAIBaseClient::setApiKey(const QString &apiKey) {
    m_apiKey = apiKey;
}

void OpenAIBaseClient::setModel(const QString &model) {
    m_model = model;
}

QString OpenAIBaseClient::model() const {
    return m_model;
}

void OpenAIBaseClient::setApiUrl(const QString &apiUrl) {
    m_apiUrl = apiUrl;
}

QString OpenAIBaseClient::apiUrl() const {
    return m_apiUrl;
}

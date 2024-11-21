
#ifndef OPENAIBASECLIENT_H
#define OPENAIBASECLIENT_H

#include <QObject>
#include <QNetworkAccessManager>

class OpenAIBaseClient : public QObject {
    Q_OBJECT

public:
    explicit OpenAIBaseClient(QObject *parent = nullptr);

    void setApiKey(const QString &apiKey);
    void setModel(const QString &model);
    QString model() const;
    void setApiUrl(const QString &apiUrl);
    QString apiUrl() const;

protected:
    QString m_apiKey;
    QString m_model;
    QString m_apiUrl;
    QNetworkAccessManager *m_networkManager;
};

#endif // OPENAIBASECLIENT_H

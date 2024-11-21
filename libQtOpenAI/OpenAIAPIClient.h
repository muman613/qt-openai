
#ifndef OPENAIAPICLIENT_H
#define OPENAIAPICLIENT_H

#include "OpenAIBaseClient.h"

class OpenAIAPIClient : public OpenAIBaseClient {
    Q_OBJECT

public:
    explicit OpenAIAPIClient(QObject *parent = nullptr);

    void sendMessage(const QString &message);

signals:
    void responseReceived(const QString &response);
    void errorOccurred(const QString &errorString);

private slots:
    void handleNetworkReply(QNetworkReply *reply);
};

#endif // OPENAIAPICLIENT_H

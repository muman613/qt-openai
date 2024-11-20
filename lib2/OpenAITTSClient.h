
#ifndef OPENAITTSCLIENT_H
#define OPENAITTSCLIENT_H

#include "OpenAIBaseClient.h"

class OpenAITTSClient : public OpenAIBaseClient {
    Q_OBJECT

public:
    explicit OpenAITTSClient(QObject *parent = nullptr);

    void generateSpeech(const QString &text);

signals:
    void audioGenerated(const QString &filePath);
    void errorOccurred(const QString &errorString);

private slots:
    void handleTTSReply(QNetworkReply *reply);
};

#endif // OPENAITTSCLIENT_H

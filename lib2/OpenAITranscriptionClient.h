
#ifndef OPENAITRANSCRIPTIONCLIENT_H
#define OPENAITRANSCRIPTIONCLIENT_H

#include "OpenAIBaseClient.h"

class OpenAITranscriptionClient : public OpenAIBaseClient {
    Q_OBJECT

public:
    explicit OpenAITranscriptionClient(QObject *parent = nullptr);

    void setLanguage(const QString &language);
    void transcribeAudio(const QString &filePath);

signals:
    void transcriptionCompleted(const QString &transcription);
    void errorOccurred(const QString &errorString);

private slots:
    void handleTranscriptionReply(QNetworkReply *reply);

private:
    QString m_language;
};

#endif // OPENAITRANSCRIPTIONCLIENT_H

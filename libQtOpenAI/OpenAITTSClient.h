#ifndef OPENAITTSCLIENT_H
#define OPENAITTSCLIENT_H

#include "OpenAIBaseClient.h"

class OpenAITTSClient : public OpenAIBaseClient {
    Q_OBJECT

public:
    explicit OpenAITTSClient(QObject *parent = nullptr);

    void generateSpeech(const QString &text);

    // New setters
    void setDownloadPath(const QString &path);
    QString downloadPath() const;

    void setFileName(const QString &fileName);
    QString fileName() const;

signals:
    void audioGenerated(const QString &filePath);
    void errorOccurred(const QString &errorString);

private slots:
    void handleTTSReply(QNetworkReply *reply);

private:
    QString m_downloadPath;    // Directory to save the audio file
    QString m_fileName;        // Name of the audio file
};

#endif // OPENAITTSCLIENT_H

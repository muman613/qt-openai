
#ifndef OPENAIIMAGEGENERATOR_H
#define OPENAIIMAGEGENERATOR_H

#include "OpenAIBaseClient.h"

class OpenAIImageGenerator : public OpenAIBaseClient {
    Q_OBJECT

public:
    explicit OpenAIImageGenerator(QObject *parent = nullptr);

    void generateImage(const QString &prompt);

signals:
    void imageGenerated(const QString &filePath);
    void errorOccurred(const QString &errorString);

private slots:
    void handleImageGenerationReply(QNetworkReply *reply);
};

#endif // OPENAIIMAGEGENERATOR_H

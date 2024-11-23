#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include "OpenAITTSClient.h"
#include <QProcessEnvironment>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Retrieve the API key from the environment variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = env.value("OPENAI_API_KEY");

    if (apiKey.isEmpty()) {
        qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
        return -1;
    }


    OpenAITTSClient ttsClient;
    ttsClient.setApiKey(apiKey);

    // imageGenerator.setDownloadPath(QDir::currentPath());
    // imageGenerator.setOutputFileName("generated_image.png");
    
    QObject::connect(&ttsClient, &OpenAITTSClient::audioGenerated, [](const QString &filePath) {
        qDebug() << "Audio generated and saved to:" << filePath;
        QCoreApplication::quit();
    });

    QObject::connect(&ttsClient, &OpenAITTSClient::errorOccurred, [](const QString &error) {
        qWarning() << "Error:" << error;
        QCoreApplication::quit();
    });
    
    ttsClient.setFileName("output");
    ttsClient.generateSpeech("This is a test, this is only a test, do not be alarmed because this is only a test. Life in the fast lane.");
    
    return app.exec();
}

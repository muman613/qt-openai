#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include "OpenAITTSClient.h"
#include <QProcessEnvironment>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Retrieve the API key from the environment variable
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
    
    ttsClient.generateSpeech("This is a test, this is only a test, do not be alarmed because this is only a test");
    
    // QObject::connect(&imageGenerator, &OpenAIImageGenerator::errorOccurred, [](const QString &error) {
    //     qWarning() << "Error:" << error;
    //     QCoreApplication::quit();
    // });

    // // Generate an image with a prompt
    // imageGenerator.generateImage("A happy kitten playing with toys");

    return app.exec();
}

#include <QCoreApplication>
#include <QDebug>
#include "OpenAIImageGenerator.h"
#include <QProcessEnvironment>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Retrieve the API key from the environment variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = env.value("OPENAI_API_KEY");

    if (apiKey.isEmpty())
    {
        qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
        return -1;
    }

    OpenAIImageGenerator imageGenerator;
    imageGenerator.setApiKey(apiKey);
    QObject::connect(&imageGenerator, &OpenAIImageGenerator::imageGenerated, [](const QString &filePath)
                     {
        qDebug() << "Image generated and saved to:" << filePath;
        QCoreApplication::quit(); });

    QObject::connect(&imageGenerator, &OpenAIImageGenerator::errorOccurred, [](const QString &error)
                     {
        qWarning() << "Error:" << error;
        QCoreApplication::quit(); });

    // Generate an image with a prompt
    imageGenerator.generateImage("A happy kitten playing with toys");

    return app.exec();
}

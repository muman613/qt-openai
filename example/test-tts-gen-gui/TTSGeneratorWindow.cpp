#include "TTSGeneratorWindow.h"
#include <QDebug>
#include <QLabel>
#include <QProcessEnvironment>
#include <QDir>

TTSGeneratorWindow::TTSGeneratorWindow(QWidget *parent)
    : QWidget(parent),
      fileNameEdit(new QLineEdit(this)),
      textEdit(new QTextEdit(this)),
      generateButton(new QPushButton("Generate", this)),
      ttsClient(new OpenAITTSClient(this)),
      mediaPlayer(new QMediaPlayer(this)) {

    // Set up the layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // File name input layout
    QVBoxLayout *fileNameLayout = new QVBoxLayout();
    fileNameLayout->addWidget(new QLabel("File Name:", this));
    fileNameLayout->addWidget(fileNameEdit);

    // Text input layout
    QVBoxLayout *textInputLayout = new QVBoxLayout();
    textInputLayout->addWidget(new QLabel("Text to Convert to Speech:", this));
    textInputLayout->addWidget(textEdit);

    // Add inputs and button to the main layout
    mainLayout->addLayout(fileNameLayout);
    mainLayout->addLayout(textInputLayout);
    mainLayout->addWidget(generateButton);

    // Connect the generate button to the TTS client
    connect(generateButton, &QPushButton::clicked, this, &TTSGeneratorWindow::onGenerateButtonClicked);

    // Connect signals from the TTS client
    connect(ttsClient, &OpenAITTSClient::audioGenerated, this, &TTSGeneratorWindow::onAudioGenerated);
    connect(ttsClient, &OpenAITTSClient::errorOccurred, this, &TTSGeneratorWindow::onErrorOccurred);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = env.value("OPENAI_API_KEY");

    if (apiKey.isEmpty()) {
        qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
    }
    
    ttsClient->setApiKey(apiKey);
}

void TTSGeneratorWindow::onGenerateButtonClicked() {
    QString fileName = fileNameEdit->text().trimmed();
    QString text = textEdit->toPlainText().trimmed();

    if (fileName.isEmpty() || text.isEmpty()) {
        qCritical() << "File name or text to convert cannot be empty.";
        return;
    }

    // Configure the TTS client
    ttsClient->setFileName(fileName);
    ttsClient->setDownloadPath(QDir::currentPath());

    // Start the text-to-speech process
    ttsClient->generateSpeech(text);
    generateButton->setEnabled(false); // Disable the button to prevent multiple requests
}

void TTSGeneratorWindow::onAudioGenerated(const QString &filePath) {
    qDebug() << "Audio successfully generated at:" << filePath;

    // Play the generated audio file
    mediaPlayer->setMedia(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();

    generateButton->setEnabled(true); // Re-enable the button
}

void TTSGeneratorWindow::onErrorOccurred(const QString &error) {
    qCritical() << "Error occurred:" << error;
    generateButton->setEnabled(true); // Re-enable the button
}

#ifndef TTSGENERATORWINDOW_H
#define TTSGENERATORWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "OpenAITTSClient.h"

class TTSGeneratorWindow : public QWidget {
    Q_OBJECT

public:
    explicit TTSGeneratorWindow(QWidget *parent = nullptr);

private slots:
    void onGenerateButtonClicked();
    void onAudioGenerated(const QString &filePath);
    void onErrorOccurred(const QString &error);

private:
    QLineEdit *fileNameEdit;    // Input for file name
    QTextEdit *textEdit;        // Input for the text to convert to speech
    QPushButton *generateButton;
    OpenAITTSClient *ttsClient; // TTS client
    QMediaPlayer *mediaPlayer;  // Audio playback
};

#endif // TTSGENERATORWINDOW_H

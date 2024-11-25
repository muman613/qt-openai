#ifndef CHATCLIENTWINDOW_H
#define CHATCLIENTWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "OpenAIAPIClient.h"

class ChatClientWindow : public QWidget {
    Q_OBJECT

public:
    explicit ChatClientWindow(QWidget *parent = nullptr);

private slots:
    void onSendButtonClicked();
    void onResponseReceived(const QString &response);
    void onErrorOccurred(const QString &error);

private:
    QTextEdit *promptEdit;      // Input for chat prompt
    QTextEdit *responseEdit;    // Output for chat response
    QPushButton *sendButton;    // Button to send prompt
    OpenAIAPIClient *chatClient; // OpenAI chat client
};

#endif // CHATCLIENTWINDOW_H

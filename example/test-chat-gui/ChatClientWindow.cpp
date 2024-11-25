#include "ChatClientWindow.h"
#include <QDebug>
#include <QProcessEnvironment>

ChatClientWindow::ChatClientWindow(QWidget *parent)
    : QWidget(parent),
      promptEdit(new QTextEdit(this)),
      responseEdit(new QTextEdit(this)),
      sendButton(new QPushButton("Send", this)),
      chatClient(new OpenAIAPIClient(this)) {

    // Set up the layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Prompt entry
    QLabel *promptLabel = new QLabel("Enter Prompt:", this);
    mainLayout->addWidget(promptLabel);
    mainLayout->addWidget(promptEdit);

    // Send button
    mainLayout->addWidget(sendButton);

    // Response output
    QLabel *responseLabel = new QLabel("Response:", this);
    mainLayout->addWidget(responseLabel);
    responseEdit->setReadOnly(true);
    mainLayout->addWidget(responseEdit);

    // Connect the send button to the chat client
    connect(sendButton, &QPushButton::clicked, this, &ChatClientWindow::onSendButtonClicked);

    // Connect signals from the chat client
    connect(chatClient, &OpenAIAPIClient::responseReceived, this, &ChatClientWindow::onResponseReceived);
    connect(chatClient, &OpenAIAPIClient::errorOccurred, this, &ChatClientWindow::onErrorOccurred);

    // Retrieve the API key from the environment variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = env.value("OPENAI_API_KEY");

    if (apiKey.isEmpty()) {
        qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
    }

    // Set default API key for the image generator
    chatClient->setApiKey(apiKey); // Replace with your OpenAI API key
}

void ChatClientWindow::onSendButtonClicked() {
    QString prompt = promptEdit->toPlainText().trimmed();

    if (prompt.isEmpty()) {
        qCritical() << "Prompt cannot be empty.";
        return;
    }

    // Send the prompt to the chat client
    chatClient->sendMessage(prompt);
    sendButton->setEnabled(false); // Disable the button to prevent multiple requests
    responseEdit->setText("Waiting for response...");
}

void ChatClientWindow::onResponseReceived(const QString &response) {
    qDebug() << "Response received:" << response;

    // Set Markdown content to the document
    responseEdit->document()->setMarkdown(response);
    sendButton->setEnabled(true); // Re-enable the button
}

void ChatClientWindow::onErrorOccurred(const QString &error) {
    qCritical() << "Error occurred:" << error;

    // Display the error message
    responseEdit->setText("Error: " + error);
    sendButton->setEnabled(true); // Re-enable the button
}

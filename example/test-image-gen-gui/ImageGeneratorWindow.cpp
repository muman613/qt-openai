#include "ImageGeneratorWindow.h"
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QProcessEnvironment>

ImageGeneratorWindow::ImageGeneratorWindow(QWidget *parent)
    : QWidget(parent), fileNameEdit(new QLineEdit(this)), descriptionEdit(new QLineEdit(this)),
      generateButton(new QPushButton("Generate", this)), imageLabel(new QLabel(this)),
      imageGenerator(new OpenAIImageGenerator(this)) {

    // Set up the layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Input fields layout
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(new QLabel("File Name:", this));
    inputLayout->addWidget(fileNameEdit);
    inputLayout->addWidget(new QLabel("Description:", this));
    inputLayout->addWidget(descriptionEdit);

    mainLayout->addLayout(inputLayout);

    // Generate button
    mainLayout->addWidget(generateButton);

    // Image display area
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setText("Generated image will appear here.");
    imageLabel->setStyleSheet("QLabel { background-color : lightgray; }");
    mainLayout->addWidget(imageLabel);

    // Connect the generate button to the generator
    connect(generateButton, &QPushButton::clicked, this, &ImageGeneratorWindow::onGenerateButtonClicked);

    // Connect signals from the OpenAIImageGenerator
    connect(imageGenerator, &OpenAIImageGenerator::imageGenerated, this, &ImageGeneratorWindow::onImageGenerated);
    connect(imageGenerator, &OpenAIImageGenerator::errorOccurred, this, &ImageGeneratorWindow::onErrorOccurred);

    // Retrieve the API key from the environment variable
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString apiKey = env.value("OPENAI_API_KEY");

    if (apiKey.isEmpty()) {
        qWarning() << "Error: OPENAI_API_KEY environment variable is not set.";
    }

    // Set default API key for the image generator
    imageGenerator->setApiKey(apiKey); // Replace with your OpenAI API key
}

void ImageGeneratorWindow::onGenerateButtonClicked() {
    QString fileName = fileNameEdit->text().trimmed();
    QString description = descriptionEdit->text().trimmed();

    if (fileName.isEmpty() || description.isEmpty()) {
        qCritical() << "File name or description cannot be empty.";
        return;
    }

    // Configure the image generator
    imageGenerator->setOutputFileName(fileName);
    imageGenerator->setDownloadPath(QDir::currentPath());

    // Start generating the image
    imageGenerator->generateImage(description);
    generateButton->setEnabled(false); // Disable button to prevent multiple requests
    imageLabel->setText("Generating image...");
}

void ImageGeneratorWindow::onImageGenerated(const QString &filePath) {
    qDebug() << "Image successfully generated at:" << filePath;

    // Load and display the image
    QPixmap pixmap(filePath);
    if (!pixmap.isNull()) {
        imageLabel->setPixmap(pixmap);
    } else {
        imageLabel->setText("Failed to load image.");
    }

    generateButton->setEnabled(true); // Re-enable the button
}

void ImageGeneratorWindow::onErrorOccurred(const QString &error) {
    qCritical() << "Error occurred:" << error;
    imageLabel->setText("Error: " + error);
    generateButton->setEnabled(true); // Re-enable the button
}

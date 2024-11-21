#ifndef IMAGEGENERATORWINDOW_H
#define IMAGEGENERATORWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "OpenAIImageGenerator.h"

class ImageGeneratorWindow : public QWidget {
    Q_OBJECT

public:
    explicit ImageGeneratorWindow(QWidget *parent = nullptr);

private slots:
    void onGenerateButtonClicked();
    void onImageGenerated(const QString &filePath);
    void onErrorOccurred(const QString &error);

private:
    QLineEdit *fileNameEdit;
    QLineEdit *descriptionEdit;
    QPushButton *generateButton;
    QLabel *imageLabel;
    OpenAIImageGenerator *imageGenerator;
};

#endif // IMAGEGENERATORWINDOW_H

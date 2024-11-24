#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QMutex>

class ImageDownloader : public QObject {
    Q_OBJECT

public:
    explicit ImageDownloader(const QString &url, QObject *parent = nullptr);

    void setDownloadPath(const QString &path);
    void setDownloadFileName(const QString &fileName);
    void setTimeout(int milliseconds); // New method to set timeout
    void start();

signals:
    void downloadComplete(const QString &downloadFilePath);
    void downloadFailed(const QString &reason);

private slots:
    void onDownloadFinished();
    void onReplyError(QNetworkReply::NetworkError code);
    void onTimeout(); // New slot for timeout handling

private:
    QNetworkAccessManager *manager = nullptr;
    QNetworkReply *currentReply = nullptr; // Track the active reply
    QString imageUrl;
    QString downloadPath;
    QString downloadFileName;

    QTimer timeoutTimer; // Timer for handling timeouts
    int timeoutDuration; // Duration for timeout in milliseconds

    QMutex mutex; // Protects shared resources for thread safety

    void cleanUpReply();
};

#endif // IMAGEDOWNLOADER_H

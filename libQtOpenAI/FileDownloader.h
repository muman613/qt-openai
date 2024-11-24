#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QMutex>
#include <QRunnable>

class FileDownloader : public QObject, public QRunnable {
    Q_OBJECT

public:
    explicit FileDownloader(const QString &url, QObject *parent = nullptr);

    void setDownloadPath(const QString &path);
    void setDownloadFileName(const QString &fileName);
    void setTimeout(int milliseconds);
    void run() override; // QRunnable: Executes in thread pool

signals:
    void downloadComplete(const QString &downloadFilePath);
    void downloadFailed(const QString &reason);

private slots:
    void onDownloadFinished();
    void onReplyError(QNetworkReply::NetworkError code);
    void onTimeout();

private:
    QNetworkAccessManager *manager;
    QNetworkReply *currentReply;
    QString fileUrl;
    QString downloadPath;
    QString downloadFileName;

    QTimer timeoutTimer;
    int timeoutDuration;

    void cleanUpReply();
};

#endif // FILEDOWNLOADER_H

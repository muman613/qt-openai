#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ImageDownloader : public QObject {
    Q_OBJECT

public:
    explicit ImageDownloader(const QString &url, QObject *parent = nullptr);

    void setDownloadPath(const QString &path);
    void setDownloadFileName(const QString &fileName);
    void start();

signals:
    void downloadComplete(const QString &downloadFilePath);
    void downloadFailed(const QString &reason);

private slots:
    void onDownloadFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    QString imageUrl;
    QString downloadPath;
    QString downloadFileName;
};

#endif // IMAGEDOWNLOADER_H

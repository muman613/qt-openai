#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QQueue>
#include <QThreadPool>
#include "FileDownloader.h"

class DownloadManager : public QObject {
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);
    ~DownloadManager();

    void addDownload(const QString &url, const QString &path, const QString &fileName);
    void setMaxConcurrentDownloads(int max);

signals:
    void allDownloadsComplete();
    void downloadProgress(int completed, int total);

private slots:
    void onDownloadComplete(const QString &filePath);
    void onDownloadFailed(const QString &reason);

private:
    QQueue<FileDownloader *> downloadQueue; // Queue of pending downloads
    QThreadPool threadPool;                // Thread pool for concurrency

    int activeDownloads;                   // Currently active downloads
    int totalDownloads;                    // Total downloads to process
    int completedDownloads;                // Downloads successfully completed
    int failedDownloads;                   // Downloads that failed

    void processNextDownload();
};

#endif // DOWNLOADMANAGER_H

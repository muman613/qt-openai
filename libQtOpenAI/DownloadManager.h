#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QQueue>
#include "FileDownloader.h"

class DownloadManager : public QObject {
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);
    ~DownloadManager();

    void addDownload(const QString &url, const QString &path, const QString &fileName);
    void setMaxConcurrentDownloads(int max);
    void setFileNamingMode(FileDownloader::NamingMode mode);
    void setFileExtension(const QString &extension);

signals:
    void allDownloadsComplete();
    void downloadProgress(int completed, int total);

private slots:
    void onDownloadComplete(const QString &filePath);
    void onDownloadFailed(const QString &reason);

private:
    QQueue<FileDownloader *> downloadQueue; // Queue of pending downloads
    int maxConcurrentDownloads;            // Max concurrent downloads allowed
    int activeDownloads;                   // Number of active downloads
    int totalDownloads;                    // Total downloads to process
    int completedDownloads;                // Count of completed downloads
    int failedDownloads;                   // Count of failed downloads
    int sequenceNumber;                    // Sequence number for SequenceNumber naming mode

    FileDownloader::NamingMode namingMode; // Current file naming mode
    QString fileExtension;                 // File extension for downloaded files

    void processNextDownload();            // Starts the next download in the queue
    void cleanupDownloader(QThread *thread, FileDownloader *downloader); // Cleans up resources
};

#endif // DOWNLOADMANAGER_H

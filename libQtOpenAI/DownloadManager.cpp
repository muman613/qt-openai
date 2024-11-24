#include "DownloadManager.h"
#include <QDebug>

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent),
      activeDownloads(0),
      totalDownloads(0),
      completedDownloads(0),
      failedDownloads(0) {
    threadPool.setMaxThreadCount(4); // Default to 4 concurrent downloads
}

DownloadManager::~DownloadManager() {
    threadPool.waitForDone(); // Ensure all tasks are completed before destruction
}

void DownloadManager::addDownload(const QString &url, const QString &path, const QString &fileName) {
    auto *downloader = new FileDownloader(url);
    downloader->setDownloadPath(path);
    downloader->setDownloadFileName(fileName);

    // Connect signals for download completion and failure
    connect(downloader, &FileDownloader::downloadComplete, this, &DownloadManager::onDownloadComplete);
    connect(downloader, &FileDownloader::downloadFailed, this, &DownloadManager::onDownloadFailed);

    downloadQueue.enqueue(downloader);
    ++totalDownloads;

    processNextDownload(); // Start processing downloads if the pool allows
}

void DownloadManager::setMaxConcurrentDownloads(int max) {
    threadPool.setMaxThreadCount(max);
}

void DownloadManager::processNextDownload() {
    if (activeDownloads >= threadPool.maxThreadCount() || downloadQueue.isEmpty()) {
        // Either we're at max concurrency or there's nothing to process
        if (completedDownloads + failedDownloads == totalDownloads && activeDownloads == 0) {
            emit allDownloadsComplete();
        }
        return;
    }

    auto *downloader = downloadQueue.dequeue();
    ++activeDownloads;

    threadPool.start(downloader); // Run downloader in the thread pool
}

void DownloadManager::onDownloadComplete(const QString &filePath) {
    qDebug() << "Download completed successfully:" << filePath;

    ++completedDownloads;
    --activeDownloads;

    emit downloadProgress(completedDownloads + failedDownloads, totalDownloads);

    if (completedDownloads + failedDownloads == totalDownloads) {
        emit allDownloadsComplete();
    } else {
        processNextDownload(); // Process the next download in the queue
    }
}

void DownloadManager::onDownloadFailed(const QString &reason) {
    qDebug() << "Download failed:" << reason;

    ++failedDownloads;
    --activeDownloads;

    emit downloadProgress(completedDownloads + failedDownloads, totalDownloads);

    if (completedDownloads + failedDownloads == totalDownloads) {
        emit allDownloadsComplete();
    } else {
        processNextDownload(); // Process the next download in the queue
    }
}

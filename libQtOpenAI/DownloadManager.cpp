#include "DownloadManager.h"
#include <QThread>
#include <QDebug>

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent),
      maxConcurrentDownloads(3),
      activeDownloads(0),
      totalDownloads(0),
      completedDownloads(0),
      failedDownloads(0),
      sequenceNumber(0),
      namingMode(FileDownloader::SimpleName) {}

DownloadManager::~DownloadManager() {}

void DownloadManager::addDownload(const QString &url, const QString &path, const QString &fileName) {
    auto *downloader = new FileDownloader(url);
    downloader->setDownloadPath(path);
    downloader->setDownloadFileName(fileName);
    downloader->setFileNamingMode(namingMode);
    downloader->setFileExtension(fileExtension);

    if (namingMode == FileDownloader::SequenceNumber) {
        downloader->setSequenceStart(sequenceNumber++);
    }

    connect(downloader, &FileDownloader::downloadComplete, this, &DownloadManager::onDownloadComplete);
    connect(downloader, &FileDownloader::downloadFailed, this, &DownloadManager::onDownloadFailed);

    downloadQueue.enqueue(downloader);
    ++totalDownloads;

    processNextDownload();
}

void DownloadManager::setMaxConcurrentDownloads(int max) {
    maxConcurrentDownloads = max;
}

void DownloadManager::setFileNamingMode(FileDownloader::NamingMode mode) {
    namingMode = mode;
    if (mode == FileDownloader::SequenceNumber) {
        sequenceNumber = 0; // Reset sequence number for new downloads
    }
}

void DownloadManager::setFileExtension(const QString &extension) {
    fileExtension = extension;
}

void DownloadManager::processNextDownload() {
    if (activeDownloads >= maxConcurrentDownloads || downloadQueue.isEmpty()) {
        if (completedDownloads + failedDownloads == totalDownloads) {
            emit allDownloadsComplete();
        }
        return;
    }

    // Get the next downloader from the queue
    auto *downloader = downloadQueue.dequeue();
    QThread *thread = new QThread;

    downloader->moveToThread(thread);

    // Ensure proper cleanup after the thread finishes
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    // Start the downloader when the thread starts
    connect(thread, &QThread::started, downloader, &FileDownloader::start);

    // Handle download completion
    connect(downloader, &FileDownloader::downloadComplete, this, [=](const QString &filePath) {
        qDebug() << "Download completed successfully:" << filePath;

        ++completedDownloads;
        --activeDownloads;

        emit downloadProgress(completedDownloads + failedDownloads, totalDownloads);

        // Cleanup downloader and thread
        cleanupDownloader(thread, downloader);

        // Continue processing the next download
        if (completedDownloads + failedDownloads == totalDownloads) {
            emit allDownloadsComplete();
        } else {
            processNextDownload();
        }
    });

    // Handle download failure
    connect(downloader, &FileDownloader::downloadFailed, this, [=](const QString &reason) {
        qDebug() << "Download failed:" << reason;

        ++failedDownloads;
        --activeDownloads;

        emit downloadProgress(completedDownloads + failedDownloads, totalDownloads);

        // Cleanup downloader and thread
        cleanupDownloader(thread, downloader);

        // Continue processing the next download
        if (completedDownloads + failedDownloads == totalDownloads) {
            emit allDownloadsComplete();
        } else {
            processNextDownload();
        }
    });

    ++activeDownloads;
    thread->start();
}

void DownloadManager::onDownloadComplete(const QString &filePath) {
    // This slot is triggered by `downloadComplete` signal
    qDebug() << "Download complete signal received for:" << filePath;
}

void DownloadManager::onDownloadFailed(const QString &reason) {
    // This slot is triggered by `downloadFailed` signal
    qDebug() << "Download failed signal received with reason:" << reason;
}

void DownloadManager::cleanupDownloader(QThread *thread, FileDownloader *downloader) {
    // Ensure signals are disconnected
    downloader->disconnect(this);

    // Stop and delete the thread
    thread->quit();
    thread->wait();
    thread->deleteLater();

    // Delete the downloader
    downloader->deleteLater();
}

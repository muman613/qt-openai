#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QTimer>
#include "DownloadManager.h"

void printUsage(const QString &appName) {
    qDebug() << "Usage:";
    qDebug() << appName << " <input_file>";
    qDebug() << "Input file format: <URL>,<RootFilename>";
    qDebug() << "Example:";
    qDebug() << "https://example.com/image1.jpg,image1";
    qDebug() << "https://example.com/image2.jpg,image2";
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Validate arguments
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    QString inputFilePath = argv[1];
    QFile inputFile(inputFilePath);

    // Open the input file
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Error: Unable to open input file:" << inputFilePath;
        return 1;
    }

    DownloadManager manager;
    manager.setMaxConcurrentDownloads(3); // Allow up to 3 parallel downloads
    manager.setFileNamingMode(FileDownloader::WithExtension); // Use sequence-based naming
    manager.setFileExtension("jpg"); // Set default file extension

    // Connect signals to track progress and completion
    QObject::connect(&manager, &DownloadManager::downloadProgress, [](int completed, int total) {
        qDebug() << "Progress: Completed" << completed << "of" << total;
    });

    QObject::connect(&manager, &DownloadManager::allDownloadsComplete, [&app]() {
        qDebug() << "All downloads complete!";
        QTimer::singleShot(500, &app, &QCoreApplication::quit); // Delay exit for cleanup
    });

    QString downloadPath = QDir::currentPath() + "/downloads";
    QDir dir(downloadPath);

    // Ensure the download directory exists
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qCritical() << "Error: Failed to create download directory:" << downloadPath;
            return 1;
        }
    }

    QTextStream in(&inputFile);
    int addedDownloads = 0;

    // Read URLs and filenames from the input file
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || !line.contains(',')) {
            qDebug() << "Skipping invalid line:" << line;
            continue;
        }

        QStringList parts = line.split(',');
        if (parts.size() != 2) {
            qDebug() << "Skipping invalid line:" << line;
            continue;
        }

        QString url = parts[0].trimmed();
        QString rootFilename = parts[1].trimmed();

        if (url.isEmpty() || rootFilename.isEmpty()) {
            qDebug() << "Skipping incomplete line:" << line;
            continue;
        }

        manager.addDownload(url, downloadPath, rootFilename);
        ++addedDownloads;
    }

    inputFile.close();

    // If no valid downloads were added, exit
    if (addedDownloads == 0) {
        qDebug() << "No valid downloads found in the input file.";
        return 0;
    }

    qDebug() << "Queued" << addedDownloads << "downloads. Starting...";
    return app.exec();
}

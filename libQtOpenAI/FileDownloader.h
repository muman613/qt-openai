#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class FileDownloader : public QObject {
    Q_OBJECT

public:
    enum NamingMode {
        SimpleName,         // filepath/filename
        WithExtension,      // filepath/filename + extension
        SequenceNumber,     // filepath/filename + "_XXX" + extension
        WithDatestamp       // filepath/filename + "_YYYYMMDD" + extension
    };
    Q_ENUM(NamingMode)

    explicit FileDownloader(const QString &url, QObject *parent = nullptr);

    void setDownloadPath(const QString &path);
    void setDownloadFileName(const QString &fileName);
    void setFileExtension(const QString &extension);
    void setFileNamingMode(NamingMode mode);
    void setSequenceStart(int start = 0); // For sequence-based naming
    void setTimeout(int milliseconds);

public slots:
    void start(); // Starts the download

signals:
    void downloadComplete(const QString &downloadFilePath);
    void downloadFailed(const QString &reason);

private slots:
    void onDownloadFinished();
    void onReplyError(QNetworkReply::NetworkError code);
    void onTimeout();

private:
    QNetworkAccessManager *manager; // Manages the network requests
    QNetworkReply *currentReply;    // Tracks the active network reply
    QTimer *timeoutTimer;           // Timer for download timeouts
    QString fileUrl;                // URL of the file to download
    QString downloadPath;           // Path to save the file
    QString downloadFileName;       // Base name for the file
    QString fileExtension;          // File extension for the file
    NamingMode namingMode;          // Selected naming mode
    int sequenceNumber;             // Sequence number for SequenceNumber mode
    int timeoutDuration;            // Timeout duration in milliseconds

    QString generateFileName() const; // Generates the file name based on the naming mode
    void cleanUpReply();              // Cleans up the current reply and related resources
};

#endif // FILEDOWNLOADER_H

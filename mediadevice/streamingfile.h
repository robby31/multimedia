#ifndef STREAMINGFILE_H
#define STREAMINGFILE_H

#include <QFile>
#include <QElapsedTimer>
#include <QThread>
#include <QDebug>
#include "device.h"

class StreamingFile : public Device
{
    Q_OBJECT

public:
    explicit StreamingFile(const QString &filename, QObject *parent = Q_NULLPTR);

    void setTimeSeek(qint64 start, qint64 end) Q_DECL_OVERRIDE;
    void setRange(qint64 startByte, qint64 endByte) Q_DECL_OVERRIDE;

    qint64 bitrate() const Q_DECL_OVERRIDE;  // bitrate in kbits/sec

    qint64 size() const Q_DECL_OVERRIDE;
    bool atEnd() const Q_DECL_OVERRIDE;
    qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    qint64 pos() const Q_DECL_OVERRIDE;

    bool open() Q_DECL_OVERRIDE;
    bool isOpen() const Q_DECL_OVERRIDE;

    bool isReadyToOpen() const Q_DECL_OVERRIDE;

    QByteArray read(qint64 maxlen) Q_DECL_OVERRIDE;

signals:

public slots:
    void close() Q_DECL_OVERRIDE;

private:
    QFile m_file;
    qint64 m_bitrate = -1;
};

#endif // STREAMINGFILE_H

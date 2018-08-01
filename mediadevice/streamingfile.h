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
    explicit StreamingFile(QString filename, QObject *parent = Q_NULLPTR);

    virtual void setTimeSeek(qint64 start, qint64 end) Q_DECL_OVERRIDE;
    virtual void setRange(qint64 startByte, qint64 endByte) Q_DECL_OVERRIDE;

    bool waitForFinished(int msecs = 30000);

    virtual qint64 bitrate() const Q_DECL_OVERRIDE;  // bitrate in kbits/sec

    virtual qint64 size() const Q_DECL_OVERRIDE;
    virtual bool atEnd() const Q_DECL_OVERRIDE;
    virtual qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    virtual qint64 pos() const Q_DECL_OVERRIDE;

    virtual bool open() Q_DECL_OVERRIDE;
    virtual bool isOpen() const Q_DECL_OVERRIDE;

    virtual bool isReadyToOpen() const Q_DECL_OVERRIDE;

    virtual QByteArray read(qint64 maxlen) Q_DECL_OVERRIDE;

signals:

public slots:
    virtual void close() Q_DECL_OVERRIDE;

private:
    QFile m_file;
    qint64 m_bitrate = -1;
};

#endif // STREAMINGFILE_H

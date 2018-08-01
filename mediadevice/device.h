#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QWaitCondition>
#include <QtMath>

class Device : public QObject
{
    Q_OBJECT

public:
    explicit Device(QObject *parent = Q_NULLPTR);
    virtual ~Device() Q_DECL_OVERRIDE;

    virtual void setTimeSeek(qint64 start, qint64 end);
    qint64 timeSeekStart() const;
    qint64 timeSeekEnd() const;

    virtual void setRange(qint64 startByte, qint64 endByte);
    qint64 startByte() const;
    qint64 endByte() const;

    virtual qint64 size() const = 0;
    virtual bool atEnd() const = 0;
    virtual qint64 bytesAvailable() const = 0;
    virtual qint64 pos() const = 0;  // returns position in bytes
    virtual qint64 progress();       // returns progress in percentage

    virtual bool open() = 0;
    virtual bool isOpen() const = 0;

    virtual qint64 bitrate() const = 0;  // bitrate in kbits/sec

    qint64 maxBufferSize() const;
    void setMaxBufferSize(const qint64 &size);

    int durationBuffer()   const;
    void setDurationBuffer(int duration);

    void appendLog(const QString &msg);

    virtual bool isReadyToOpen() const = 0;
    bool waitReadyToOpen(const int &timeout=30000);
    bool waitOpen(const int &timeout=30000);

    virtual QByteArray read(qint64 maxlen) = 0;

signals:
    void LogMessage(const QString &msg);
    void status(const QString &status);
    void errorRaised(const QString &errorString);
    void openedSignal();
    void sendDataToClientSignal(const QByteArray &data);
    void endReached();
    void readyRead();
    void readyToOpen();
    void closed();
    void abort();

public slots:
    void startRequestData();
    void requestData(qint64 maxlen);
    virtual void close() = 0;

private slots:
    void deviceReadyToOpen();
    void deviceOpened();

private:
    static const QString CRLF;    // Carriage return and line feed.

    qint64 timeseek_start = -1;
    qint64 timeseek_end = -1;

    qint64 m_startByte = -1;
    qint64 m_endByte = -1;

    qint64 m_maxBufferSize = 1024*1024*10;  // 10 MBytes by default when bitrate is unknown
    int m_durationBuffer = 20;              // when bitrate is known, m_maxBufferSize is set to m_durationBuffer seconds of streaming

    bool requestDataStarted = false;

    QMutex mutex;
    QWaitCondition readyToOpenCondition;
    QWaitCondition isopenedCondition;

public:
    qint64 static objectCounter;
};

#endif // DEVICE_H

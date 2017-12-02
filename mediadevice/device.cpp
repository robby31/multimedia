#include "device.h"

qint64 Device::objectCounter = 0;

const QString Device::CRLF = "\r\n";

Device::Device(QObject *parent) :
    QObject(parent)
{
    ++objectCounter;

    connect(this, SIGNAL(readyToOpen()), this, SLOT(deviceReadyToOpen()));
    connect(this, SIGNAL(openedSignal()), this, SLOT(deviceOpened()));
}

Device::~Device()
{
    --objectCounter;
}

void Device::appendLog(const QString &msg)
{
    emit LogMessage(msg);
}

qint64 Device::progress()
{
    if (size() <= 0)
        return 0;
    else
        return qCeil(100.0*double(pos())/double(size()));
}

qint64 Device::timeSeekStart() const
{
    return timeseek_start;
}

qint64 Device::timeSeekEnd() const
{
    return timeseek_end;
}

void Device::setTimeSeek(qint64 start, qint64 end)
{
    if (start < 0 && end < 0)
    {
        qCritical() << "invalid timeSeek" << start << end;
    }
    else
    {
        if (start > 0 && end > 0 && end < start)
        {
            qCritical() << "invalid timeSeek" << start << end;
        }
        else
        {
            timeseek_start = start;
            timeseek_end = end;
        }
    }
}

qint64 Device::startByte() const
{
    return m_startByte;
}

qint64 Device::endByte() const
{
    return m_endByte;
}

void Device::setRange(qint64 startByte, qint64 endByte)
{
    if (startByte >= 0 || startByte == -1)
        m_startByte = startByte;
    else
        qCritical() << "invalid startByte" << startByte;

    if (endByte == -1 || (endByte >= 0 && endByte >= startByte))
        m_endByte = endByte;
    else
        qCritical() << "invalid endByte" << endByte;
}

qint64 Device::maxBufferSize() const
{
    return m_maxBufferSize;
}

void Device::setMaxBufferSize(const qint64 &size)
{
    if (size > 0)
        m_maxBufferSize = size;
    else
        qCritical() << "setMaxBufferSize : invalid size" << size;
}

int Device::durationBuffer() const
{
    return m_durationBuffer;
}

void Device::setDurationBuffer(int duration)
{
    if (bitrate() > 0)
    {
        if (duration > 0)
        {
            m_durationBuffer = duration;
            setMaxBufferSize(bitrate()/8*m_durationBuffer);
        }
        else
        {
            qCritical() << "invalid duration" << duration;
        }
    }
    else
    {
        qCritical() << "invalid bitrate" << bitrate() << "cannot set buffer size to duration" << duration;
    }
}

void Device::deviceOpened()
{
    appendLog(QString("device opened, %2 bytes to send, %1 bytes available.").arg(bytesAvailable()).arg(size()));
    isopenedCondition.wakeAll();
}

void Device::startRequestData()
{
    appendLog(QString("START REQUEST DATA : %1 bytes available").arg(bytesAvailable()));

    requestDataStarted = true;
    emit status("Streaming");

    requestData(m_maxBufferSize);
}

void Device::requestData(qint64 maxlen)
{
    if (requestDataStarted)
    {
        if (maxlen <= 0)
        {
            qCritical() << "invalid requested data length" << maxlen;
        }
        else
        {
            if (!atEnd() && bytesAvailable() > 0)
            {
                // read the stream
                QByteArray bytesToSend = read(maxlen);
                if (!bytesToSend.isEmpty())
                    emit sendDataToClientSignal(bytesToSend);

                if (atEnd())
                    emit endReached();
            }
            else
            {
                qDebug() << QString("%1: sendDataToClient no data available").arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz"));
            }
        }
    }
}

bool Device::waitReadyToOpen(const int &timeout)
{
    QMutexLocker locker(&mutex);

    if (!isReadyToOpen())
    {
        qDebug() << "wait device is ready to open" << this;
        return readyToOpenCondition.wait(locker.mutex(), timeout);
    }
    else
    {
        return true;
    }
}

void Device::deviceReadyToOpen()
{
    readyToOpenCondition.wakeAll();
}

bool Device::waitOpen(const int &timeout)
{
    QMutexLocker locker(&mutex);

    if (!isOpen())
    {
        qDebug() << "wait device until open" << this;
        return isopenedCondition.wait(locker.mutex(), timeout);
    }
    else
    {
        return true;
    }
}

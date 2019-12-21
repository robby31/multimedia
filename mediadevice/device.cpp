#include "device.h"

const QString Device::CRLF = "\r\n";

Device::Device(QObject *parent) :
    QObject(parent)
{
    DebugInfo::add_object(this);

    connect(this, SIGNAL(readyToOpen()), this, SLOT(deviceReadyToOpen()));
    connect(this, SIGNAL(openedSignal()), this, SLOT(deviceOpened()));
}

void Device::appendLog(const QString &msg)
{
    emit LogMessage(msg);
}

qint64 Device::progress()
{
    if (size() <= 0)
        return 0;

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
                #if !defined(QT_NO_DEBUG_OUTPUT)
                appendLog(QString("%1: sendDataToClient no data available").arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")));
                #endif
            }
        }
    }
}

bool Device::waitReadyToOpen(const unsigned long &timeout)
{
    QMutexLocker locker(&mutex);

    if (!isReadyToOpen())
    {
        #if !defined(QT_NO_DEBUG_OUTPUT)
        appendLog("wait device is ready to open");
        #endif
        return readyToOpenCondition.wait(locker.mutex(), timeout);
    }

    return true;
}

void Device::deviceReadyToOpen()
{
    readyToOpenCondition.wakeAll();
}

bool Device::waitOpen(const unsigned long &timeout)
{
    QMutexLocker locker(&mutex);

    if (!isOpen())
    {
        #if !defined(QT_NO_DEBUG_OUTPUT)
        appendLog("wait device until open");
        #endif
        return isopenedCondition.wait(locker.mutex(), timeout);
    }

    return true;
}

bool Device::waitForFinished(const int &timeout)
{
    if (bytesAvailable() > 0)
        emit readyRead();

    if (!atEnd())
    {
        QElapsedTimer timer;
        timer.start();

        QEventLoop loop;
        connect(this, &Device::endReached, &loop, &QEventLoop::quit);

        if (timeout > 0)
            QTimer::singleShot(timeout, &loop, &QEventLoop::quit);

        loop.exec(QEventLoop::ExcludeUserInputEvents);

        if (timeout > 0)
            return timer.elapsed() < timeout;
    }

    return true;
}

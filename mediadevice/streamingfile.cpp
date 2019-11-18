#include "streamingfile.h"

StreamingFile::StreamingFile(const QString &filename, QObject *parent) :
    Device(parent),
    m_file(filename, parent)
{
    emit readyToOpen();
}

void StreamingFile::setTimeSeek(qint64 start, qint64 end)
{
    qWarning() << "timeseek not applicable for StreamingFile" << start << end;
}

void StreamingFile::setRange(qint64 startByte, qint64 endByte)
{
    qint64 start = startByte;
    qint64 end = endByte;

    if (start >= m_file.size())
    {
        qWarning() << "invalid startByte (greater than file size)" << startByte << m_file.size();
        start = 0;
    }

    if (end >= m_file.size())
    {
        qWarning() << "invalid endByte (greater than file size)" << endByte << m_file.size();
        end = m_file.size() - 1;
    }

    Device::setRange(start, end);
}

qint64 StreamingFile::size() const
{
    if (endByte() != -1)
    {
        if (startByte() != -1)
            return endByte() - startByte() + 1;
    }
    else
    {
        if (startByte() != -1)
            return m_file.size() - startByte();
    }

    return m_file.size();
}

qint64 StreamingFile::bytesAvailable() const
{
    return qMin(m_file.bytesAvailable(), size() - pos());
}

qint64 StreamingFile::pos() const
{
    if (startByte() != -1 && m_file.pos() >= startByte())
        return m_file.pos() - startByte();

    return m_file.pos();
}

bool StreamingFile::atEnd() const
{
    if (!isOpen())
        return true;

    return pos() >= size();
}

QByteArray StreamingFile::read(qint64 maxlen)
{
    QByteArray res;

    if (!atEnd())
    {
        qint64 bytesToRead = size() - pos();
        if (bytesToRead>0 && bytesToRead<maxlen)
            res = m_file.read(bytesToRead);
        else if (maxlen>0)
            res = m_file.read(maxlen);
        else
            qWarning() << "no data to read";

        emit status(QString("Streaming (%1%)").arg(progress()));
    }

    return res;
}

bool StreamingFile::isOpen() const
{
    return m_file.isOpen();
}

bool StreamingFile::open()
{
    if (timeSeekStart() != -1 || timeSeekEnd() != -1)
        qWarning() << "timeseek defined not applicable for StreamingFile.";

    bool res = m_file.open(QIODevice::ReadOnly);

    if (pos() < startByte())
        if (!m_file.seek(startByte()))
            qCritical() << "Cannot seek file to position" << startByte() << "bytes.";

    if (res)
        emit openedSignal();

    return res;
}

bool StreamingFile::isReadyToOpen() const
{
    return true;
}

void StreamingFile::close()
{
    m_file.close();
    emit closed();
}


qint64 StreamingFile::bitrate() const
{
    return m_bitrate;
}

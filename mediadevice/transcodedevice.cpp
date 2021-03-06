#include "transcodedevice.h"

TranscodeDevice::TranscodeDevice(QObject *parent) :
    Device(parent),
    m_frameRate("")
{
    connect(this, SIGNAL(openSignal()), this, SLOT(_open()));
}

TranscodeDevice::~TranscodeDevice()
{
    _close();
}

void TranscodeDevice::close()
{
    _close();
}

void TranscodeDevice::_close()
{
    appendLog("device destroyed");
    m_opened = false;
    m_pos = 0;
    transcodeClock.invalidate();
    emit closed();
}

void TranscodeDevice::startTranscodingClock()
{
    transcodeClock.start();
}

qint64 TranscodeDevice::transcodingElapsed()
{
    return transcodeClock.elapsed();
}

qint64 TranscodeDevice::size() const
{
    qint64 tmp_size = fullSize();

    if (tmp_size > 0)
    {
        if (endByte() != -1)
        {
            if (startByte() != -1)
                return endByte() - startByte() + 1;
        }
        else
        {
            if (startByte() != -1 && tmp_size >= startByte())
                return tmp_size - startByte();
        }

        return tmp_size;
    }

    qCritical() << "length or bitrate is invalid" << lengthInMSeconds() << bitrate() << "full size = " << tmp_size << videoUrl() << audioUrl();
    return -1;
}

qint64 TranscodeDevice::lengthInSeconds() const
{
    return lengthInMSeconds()/1000;
}

qint64 TranscodeDevice::lengthInMSeconds() const
{
    if (timeSeekStart() > 0)
    {
        if (timeSeekEnd() > timeSeekStart() && (timeSeekEnd()*1000) < static_cast<qint64>(originalLengthInMSeconds()))
            return (timeSeekEnd() - timeSeekStart())*1000;

        return static_cast<qint64>(originalLengthInMSeconds()) - timeSeekStart()*1000;
    }

    if (timeSeekEnd() > 0 && (timeSeekEnd()*1000) < static_cast<qint64>(originalLengthInMSeconds()))
        return timeSeekEnd()*1000;

    return static_cast<qint64>(originalLengthInMSeconds());
}

qint64 TranscodeDevice::transcodedProgress() const
{
    if (size()==0)
        return 0;

    return qint64(100.0*double(transcodedPos())/double(size()));
}

void TranscodeDevice::setVolumeInfo(const QHash<QString, double>& info)
{
    m_volumeInfo = info;
}

void TranscodeDevice::setUrl(const QUrl &url)
{
    m_videoUrl = url;
    m_audioUrl = url;
    emit readyToOpen();
}

void TranscodeDevice::setUrls(const QUrl &videoUrl, const QUrl &audioUrl)
{
    m_videoUrl = videoUrl;
    m_audioUrl = audioUrl;
    emit readyToOpen();
}

void TranscodeDevice::setVideoUrl(const QUrl &url)
{
    m_videoUrl = url;
    emit readyToOpen();
}

void TranscodeDevice::setAudioUrl(const QUrl &url)
{
    m_audioUrl = url;
    emit readyToOpen();
}

QUrl TranscodeDevice::videoUrl() const
{
    return m_videoUrl;
}

QUrl TranscodeDevice::audioUrl() const
{
    return m_audioUrl;
}

void TranscodeDevice::urlError(const QString &message)
{
    // cannot define streaming url
    emit errorRaised(message);

    emit abort();
}

bool TranscodeDevice::open()
{
    if (!isReadyToOpen())
    {
        qCritical() << "url is not defined";
        return false;
    }

    emit openSignal();
    return true;
}

bool TranscodeDevice::isReadyToOpen() const
{
    return videoUrl().isValid() || audioUrl().isValid();
}

qint64 TranscodeDevice::fullSize() const
{
    if (lengthInMSeconds() > 0 && bitrate() > 0)
    {
        if (format() == MP3)
            return static_cast<qint64>(static_cast<double>(lengthInMSeconds())/1000.0*static_cast<double>(bitrate())/8.0) + 2000;   // header size = 2000 bytes

        return static_cast<qint64>(overheadfactor()*static_cast<double>(lengthInMSeconds())/1000.0*static_cast<double>(bitrate())/8.0);
    }

    return -1;
}

void TranscodeDevice::setRange(qint64 startByte, qint64 endByte)
{
    qint64 start = startByte;
    qint64 end = endByte;

    if (start >= fullSize())
    {
        qCritical() << "invalid startByte (greater than file size)" << startByte << fullSize();
        start = 0;
    }

    if (end >= fullSize())
    {
        qCritical() << "invalid endByte (greater than file size)" << endByte << fullSize();
        end = fullSize() - 1;
    }

    Device::setRange(start, end);
}

double TranscodeDevice::overheadfactor() const
{
    switch (format())
    {
    case MP3:
        return 1.0;
    case LPCM_S16BE:
        return 1.00001;
    case LPCM_S16LE:
        return 1.00001;
    case AAC:
        return 1.01;
    case ALAC:
        return 1.08;
    case H264_AAC:
    case H264_AC3:
        return 1.105;
    case MPEG2_AC3:
        return 1.0847;
    default:
        return 1.0;
    }
}

TranscodeFormatAvailable TranscodeDevice::format() const
{
    return m_format;
}

void TranscodeDevice::setFormat(const TranscodeFormatAvailable &format)
{
    m_format = format;
    emit formatChanged();
}

qint64 TranscodeDevice::pos() const
{
    if (startByte() != -1 && m_pos >= startByte())
        return m_pos - startByte();

    return m_pos;
}

ContainerType TranscodeDevice::containerFormat() const
{
    return m_containerFormat;
}

void TranscodeDevice::setContainer(const ContainerType &format)
{
    m_containerFormat = format;
    emit containerChanged();
}

#include "qffmpegstream.h"

QFfmpegStream::QFfmpegStream(QObject *parent):
    QObject(parent)
{
    DebugInfo::add_object(this);
}

QFfmpegStream::~QFfmpegStream()
{
    _close();
}

void QFfmpegStream::close()
{
    _close();
}

void QFfmpegStream::_close()
{
    m_stream = Q_NULLPTR;
}

bool QFfmpegStream::isValid() const
{
    return m_stream != Q_NULLPTR;
}

bool QFfmpegStream::setStream(AVStream *stream)
{
    if (stream != Q_NULLPTR && m_stream == Q_NULLPTR)
    {
        m_stream = stream;
        return true;
    }

    return false;
}

bool QFfmpegStream::setStream(AVStream *stream, uint streamId)
{
    if (setStream(stream))
    {
        m_stream->id = static_cast<int>(streamId);
        return true;
    }

    return false;
}

bool QFfmpegStream::setTimeBase(int num, int den)
{
    if (m_stream != Q_NULLPTR && den != 0)
    {
        m_stream->time_base.num = num;
        m_stream->time_base.den = den;
        return true;
    }

    return false;
}

int QFfmpegStream::streamIndex() const
{
    if (m_stream != Q_NULLPTR)
        return m_stream->index;

    return -1;
}

AVStream *QFfmpegStream::stream() const
{
    return m_stream;
}

AVSampleFormat QFfmpegStream::sampleFormat() const
{
    if (codec())
        return codec()->sampleFormat();

    return AV_SAMPLE_FMT_NONE;
}

AVPixelFormat QFfmpegStream::pixelFormat() const
{
    if (codec())
        return codec()->pixelFormat();

    return AV_PIX_FMT_NONE;
}

int QFfmpegStream::channelCount() const
{
    if (codec())
        return codec()->channelCount();

    return -1;
}

uint64_t QFfmpegStream::channelLayout() const
{
    if (codec())
        return codec()->channelLayout();

    return 0;
}

int QFfmpegStream::samplerate() const
{
    if (codec())
        return codec()->samplerate();

    return -1;
}

qint64 QFfmpegStream::bitrate() const
{
    if (codec())
        return codec()->bitrate();

    return -1;
}

QString QFfmpegStream::format() const
{
    if (codec())
        return codec()->format();

    return QString();
}

AVPacket *QFfmpegStream::attached_pic() const
{
    if (m_stream != Q_NULLPTR && (m_stream->disposition & AV_DISPOSITION_ATTACHED_PIC))
        return &m_stream->attached_pic;

    return Q_NULLPTR;
}

QString QFfmpegStream::resolution() const
{
    if (codec())
        return codec()->resolution();

    return QString();
}

double QFfmpegStream::frameRate() const
{
    if (m_stream != Q_NULLPTR)
    {
        AVRational frame_rate = m_stream->avg_frame_rate;
        if (frame_rate.den != 0)
            return static_cast<double>(frame_rate.num) / static_cast<double>(frame_rate.den);
    }

    return 0.0;
}

qint64 QFfmpegStream::getStartTimeInMicroSec() const
{
    if (m_stream && m_stream->start_time != AV_NOPTS_VALUE)
        return m_stream->start_time * 1000000 / AV_TIME_BASE;

    return 0;
}

double QFfmpegStream::getDurationInSec() const
{
    return getDurationInMicroSec() / 1000000.0;
}

qint64 QFfmpegStream::getDurationInMicroSec() const
{
    if (m_stream)
        return 1000000 * m_stream->time_base.num * m_stream->duration / m_stream->time_base.den;

    return 0;
}

bool QFfmpegStream::setDurationInMicroSec(const qint64 &estimated_duration_Microsec)
{
    if (m_stream != Q_NULLPTR && m_stream->time_base.num != 0)
    {
        m_stream->duration = qCeil(static_cast<double>(estimated_duration_Microsec * m_stream->time_base.den) / static_cast<double>(1000000 * m_stream->time_base.num));
        return true;
    }

    return false;
}

QString QFfmpegStream::metaData(const QString &tagName) const
{
    if (m_stream != Q_NULLPTR)
    {
        AVDictionaryEntry *entry = Q_NULLPTR;
        while ((entry = av_dict_get(m_stream->metadata, "", entry, AV_DICT_IGNORE_SUFFIX)))
        {
            if (entry != Q_NULLPTR && entry->key == tagName)
                return  entry->value;
        }
    }

    return QString();
}

qint64 QFfmpegStream::packetPosInMsec(AVPacket *pkt)
{
    if (m_stream && pkt != Q_NULLPTR && codec() && codec()->codecCtx() != Q_NULLPTR)
    {
        AVRational time_base = m_stream->time_base;
        return (pkt->pts + codec()->codecCtx()->delay) * 1000 * time_base.num / time_base.den ;
    }

    return -1;
}

qint64 QFfmpegStream::packetNextPosInMsec(AVPacket *pkt)
{
    if (m_stream && pkt != Q_NULLPTR && codec() && codec()->codecCtx() != Q_NULLPTR)
    {
        AVRational time_base = m_stream->time_base;
        return (pkt->pts + pkt->duration + codec()->codecCtx()->delay) * 1000 * time_base.num / time_base.den ;
    }

    return -1;
}

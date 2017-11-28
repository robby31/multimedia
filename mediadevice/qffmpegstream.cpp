#include "qffmpegstream.h"

qint64 QFfmpegStream::objectCounter = 0;

QFfmpegStream::QFfmpegStream()
{
    objectCounter++;
}

QFfmpegStream::~QFfmpegStream()
{
    objectCounter--;
    close();
}

void QFfmpegStream::close()
{
    m_stream = NULL;
}

bool QFfmpegStream::isValid() const
{
    return m_stream != NULL;
}

bool QFfmpegStream::setStream(AVStream *stream)
{
    if (stream != NULL && m_stream == NULL)
    {
        m_stream = stream;
        return true;
    }
    else
    {
        return false;
    }
}

bool QFfmpegStream::setStream(AVStream *stream, int streamId)
{
    if (setStream(stream))
    {
        m_stream->id = streamId;
        return true;
    }
    else
    {
        return false;
    }
}

bool QFfmpegStream::setTimeBase(int num, int den)
{
    if (m_stream != NULL && den != 0)
    {
        m_stream->time_base.num = num;
        m_stream->time_base.den = den;
        return true;
    }
    else
    {
        return false;
    }
}

int QFfmpegStream::streamIndex() const
{
    if (m_stream != NULL)
        return m_stream->index;
    else
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
    else
        return AV_SAMPLE_FMT_NONE;
}

AVPixelFormat QFfmpegStream::pixelFormat() const
{
    if (codec())
        return codec()->pixelFormat();
    else
        return AV_PIX_FMT_NONE;
}

int QFfmpegStream::channelCount() const
{
    if (codec())
        return codec()->channelCount();
    else
        return -1;
}

uint64_t QFfmpegStream::channelLayout() const
{
    if (codec())
        return codec()->channelLayout();
    else
        return -1;
}

int QFfmpegStream::samplerate() const
{
    if (codec())
        return codec()->samplerate();
    else
        return -1;
}

qint64 QFfmpegStream::bitrate() const
{
    if (codec())
        return codec()->bitrate();
    else
        return -1;
}

QString QFfmpegStream::format() const
{
    if (codec())
        return codec()->format();
    else
        return QString();
}

AVPacket *QFfmpegStream::attached_pic() const
{
    if (m_stream != NULL && (m_stream->disposition & AV_DISPOSITION_ATTACHED_PIC))
        return &m_stream->attached_pic;
    else
        return NULL;
}

QString QFfmpegStream::resolution() const
{
    if (codec())
        return codec()->resolution();
    else
        return QString();
}

double QFfmpegStream::frameRate() const
{
    if (m_stream != NULL)
    {
        AVRational frame_rate = m_stream->avg_frame_rate;
        if (frame_rate.den != 0)
            return (double) frame_rate.num / (double) frame_rate.den;
        else
            return 0.0;
    }
    else
        return 0.0;
}

qint64 QFfmpegStream::getDuration() const
{
    if (m_stream != NULL)
        return 1000 * m_stream->time_base.num * m_stream->duration / m_stream->time_base.den;
    else
     return -1;
}

QString QFfmpegStream::metaData(const QString &tagName) const
{
    if (m_stream != NULL)
    {
        AVDictionaryEntry *entry = NULL;
        while ((entry = av_dict_get(m_stream->metadata, "", entry, AV_DICT_IGNORE_SUFFIX)))
        {
            if (entry != NULL && entry->key == tagName)
                return  entry->value;
        }
    }

    return QString();
}

qint64 QFfmpegStream::packetPosInMsec(AVPacket *pkt)
{
    if (m_stream && pkt != NULL && codec() && codec()->codecCtx() != NULL)
    {
        AVRational time_base = m_stream->time_base;
        return (pkt->pts + codec()->codecCtx()->delay) * 1000 * time_base.num / time_base.den ;
    }
    else
    {
        return -1;
    }
}

qint64 QFfmpegStream::packetNextPosInMsec(AVPacket *pkt)
{
    if (m_stream && pkt != NULL && codec() && codec()->codecCtx() != NULL)
    {
        AVRational time_base = m_stream->time_base;
        return (pkt->pts + pkt->duration + codec()->codecCtx()->delay) * 1000 * time_base.num / time_base.den ;
    }
    else
    {
        return -1;
    }
}

#include "qffmpeginputstream.h"

QFfmpegInputStream::~QFfmpegInputStream()
{
    _close();
}

bool QFfmpegInputStream::isValid() const
{
    return QFfmpegStream::isValid() && m_codec && m_codec->isValid();
}

void QFfmpegInputStream::close()
{
    _close();
}

void QFfmpegInputStream::_close()
{
    if (m_codec)
    {
        delete m_codec;
        m_codec = Q_NULLPTR;
    }

    QFfmpegStream::close();
}

QFfmpegDecoder *QFfmpegInputStream::codec() const
{
    return m_codec;
}

bool QFfmpegInputStream::init_decoding_stream(AVFormatContext *format, AVMediaType type)
{
    if (format != Q_NULLPTR)
    {
        AVCodec *codec = Q_NULLPTR;
        auto index = av_find_best_stream(format, type, -1, -1, &codec, 0);

        if (index >=0 && index < static_cast<int>(format->nb_streams) && setStream(format->streams[index]))
        {
            if (type == AVMEDIA_TYPE_AUDIO)
                m_codec = new QFfmpegAudioDecoder();
            else if (type == AVMEDIA_TYPE_VIDEO)
                m_codec = new QFfmpegVideoDecoder();

            if (!m_codec || !m_codec->init_codec(codec, stream()->codecpar))
            {
                qCritical() << "unable to init codec" << codec << stream()->codecpar;
                close();
                return false;
            }

            return true;
        }

        if (index == AVERROR_STREAM_NOT_FOUND)
        {
//            qCritical() << "no stream with the requested type" << type << "could be found";
        }
        else if (index == AVERROR_DECODER_NOT_FOUND)
        {
            qCritical() << "no valid decoder for streams found with requested type" << type << ".";
        }
        else
        {
            qCritical() << "unable to set stream" << index << type << format->streams[index];
        }
    }

    close();
    return false;
}

bool QFfmpegInputStream::init_decoding_stream(AVFormatContext *format, uint index)
{
    if (format != Q_NULLPTR && index < format->nb_streams && format->streams[index] != Q_NULLPTR)
    {
        AVCodec *codec = avcodec_find_decoder(format->streams[index]->codecpar->codec_id);

        if (setStream(format->streams[index]))
        {
            if (codec->type == AVMEDIA_TYPE_AUDIO)
                m_codec = new QFfmpegAudioDecoder();
            else if (codec->type == AVMEDIA_TYPE_VIDEO)
                m_codec = new QFfmpegVideoDecoder();

            if (m_codec && m_codec->init_codec(codec, stream()->codecpar))
                return true;
        }
        else {
            qCritical() << "invalid stream" << index;
        }
    }

    close();
    return false;
}

qint64 QFfmpegInputStream::decodedFramesAvailable() const
{
    if (!m_codec)
        return 0;

    return m_codec->decodedFramesAvailable();
}

QFfmpegFrame *QFfmpegInputStream::takeDecodedFrame()
{
    if (!m_codec)
        return Q_NULLPTR;

    return m_codec->takeDecodedFrame();
}

bool QFfmpegInputStream::decodePacket(AVPacket *pkt)
{
    if (!m_codec)
        return false;

    return m_codec->decodePacket(pkt);
}

void QFfmpegInputStream::flush()
{
    if (m_codec)
        avcodec_flush_buffers(m_codec->codecCtx());
}

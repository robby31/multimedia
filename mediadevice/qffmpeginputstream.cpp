#include "qffmpeginputstream.h"

QFfmpegInputStream::QFfmpegInputStream():
    QFfmpegStream()
{

}

QFfmpegInputStream::~QFfmpegInputStream()
{
    close();
}

bool QFfmpegInputStream::isValid() const
{
    return QFfmpegStream::isValid() && m_codec && m_codec->isValid();
}

void QFfmpegInputStream::close()
{
    if (m_codec)
    {
        delete m_codec;
        m_codec = NULL;
    }

    QFfmpegStream::close();
}

QFfmpegDecoder *QFfmpegInputStream::codec() const
{
    return m_codec;
}

bool QFfmpegInputStream::init_decoding_stream(AVFormatContext *format, AVMediaType type)
{
    if (format != NULL)
    {
        AVCodec *codec;
        int index = av_find_best_stream(format, type, -1, -1, &codec, 0);

        if (index >= 0 && index < (int)format->nb_streams && setStream(format->streams[index]))
        {
            if (type == AVMEDIA_TYPE_AUDIO)
                m_codec = new QFfmpegAudioDecoder();
            else if (type == AVMEDIA_TYPE_VIDEO)
                m_codec = new QFfmpegVideoDecoder();

            if (!m_codec or !m_codec->init_codec(codec, stream()->codecpar))
            {
                close();
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            close();
            return false;
        }
    }
    else
    {
        close();
        return false;
    }
}

bool QFfmpegInputStream::init_decoding_stream(const AVCodecID id, AVFormatContext *fmtContext)
{
    if (id != AV_CODEC_ID_NONE && fmtContext != NULL)
    {
        if (setStream(avformat_new_stream(fmtContext, NULL), fmtContext->nb_streams))
        {
            AVCodec *tmpCodec = avcodec_find_encoder(id);
            if (tmpCodec != NULL && tmpCodec->type == AVMEDIA_TYPE_AUDIO)
                m_codec = new QFfmpegAudioDecoder();
            else if (tmpCodec != NULL && tmpCodec->type == AVMEDIA_TYPE_VIDEO)
                m_codec = new QFfmpegVideoDecoder();

            if (!m_codec or !m_codec->init_codec(id))
            {
                close();
                return false;
            }
            else
            {
                /* Some formats want stream headers to be separate. */
                if (fmtContext->oformat->flags & AVFMT_GLOBALHEADER)
                    m_codec->codecCtx()->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

                return true;
            }
        }
        else
        {
            close();
            return false;
        }
    }
    else
    {
        close();
        return false;
    }
}

qint64 QFfmpegInputStream::decodedFramesAvailable() const
{
    if (m_codec)
        return m_codec->decodedFramesAvailable();
    else
        return 0;
}

QFfmpegFrame *QFfmpegInputStream::takeDecodedFrame()
{
    if (m_codec)
        return m_codec->takeDecodedFrame();
    else
        return NULL;
}

bool QFfmpegInputStream::decodePacket(AVPacket *pkt)
{
    if (m_codec)
        return m_codec->decodePacket(pkt);
    else
        return false;
}

void QFfmpegInputStream::flush()
{
    if (m_codec)
        avcodec_flush_buffers(m_codec->codecCtx());
}

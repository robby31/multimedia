#include "qffmpegoutputstream.h"

QFfmpegOutputStream::~QFfmpegOutputStream()
{
    _close();
}

bool QFfmpegOutputStream::isValid() const
{
    return QFfmpegStream::isValid() && m_codec && m_codec->isValid();
}

void QFfmpegOutputStream::close()
{
    _close();
}

void QFfmpegOutputStream::_close()
{
    if (m_codec)
    {
        delete m_codec;
        m_codec = Q_NULLPTR;
    }

    QFfmpegStream::close();
}

QFfmpegEncoder *QFfmpegOutputStream::codec() const
{
    return m_codec;
}

bool QFfmpegOutputStream::init_encoding_stream(const AVCodecID &id, AVFormatContext *fmtContext)
{
    if (id != AV_CODEC_ID_NONE && fmtContext != Q_NULLPTR)
    {
        if (setStream(avformat_new_stream(fmtContext, Q_NULLPTR), fmtContext->nb_streams))
        {
            AVCodec *tmpCodec = avcodec_find_encoder(id);
            if (tmpCodec != Q_NULLPTR && tmpCodec->type == AVMEDIA_TYPE_AUDIO)
                m_codec = new QFfmpegAudioEncoder();
            else if (tmpCodec != Q_NULLPTR && tmpCodec->type == AVMEDIA_TYPE_VIDEO)
                m_codec = new QFfmpegVideoEncoder();

            if (!m_codec || !m_codec->init_codec(id))
            {
                close();
                return false;
            }

            /* Some formats want stream headers to be separate. */
            if (fmtContext->oformat->flags & AVFMT_GLOBALHEADER)
                m_codec->codecCtx()->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            return true;
        }
    }

    close();
    return false;
}

bool QFfmpegOutputStream::openOutput()
{
    if (m_codec && m_codec->codecCtx() != Q_NULLPTR)
    {
        if (m_codec->type() == AVMEDIA_TYPE_AUDIO)
        {
            setTimeBase(1, samplerate());
        }
        else if (m_codec->type() == AVMEDIA_TYPE_VIDEO)
        {
            m_codec->codecCtx()->time_base.num = 1;
            m_codec->codecCtx()->time_base.den = 25;
            setTimeBase(m_codec->codecCtx()->time_base.num, m_codec->codecCtx()->time_base.den);
        }

        return m_codec->open() && (avcodec_parameters_from_context(stream()->codecpar, m_codec->codecCtx()) >= 0);
    }

    return false;
}

bool QFfmpegOutputStream::setSampleFmt(const AVSampleFormat &format)
{
    if (m_codec)
        return m_codec->setSampleFmt(format);

    return false;
}

bool QFfmpegOutputStream::setPixelFormat(const AVPixelFormat &format)
{
    if (m_codec)
        return m_codec->setPixelFormat(format);

    return false;
}

bool QFfmpegOutputStream::setChannelLayout(const uint64_t &layout)
{
    if (m_codec)
        return m_codec->setChannelLayout(layout);

    return false;
}

bool QFfmpegOutputStream::setSampleRate(const int &rate)
{
    if (m_codec)
        return m_codec->setSampleRate(rate);

    return false;
}

bool QFfmpegOutputStream::setBitRate(const qint64 &bitrate)
{
    if (m_codec)
        return m_codec->setBitRate(bitrate);

    return false;
}

qint64 QFfmpegOutputStream::encodedPktAvailable() const
{
    if (m_codec)
        return m_codec->encodedPktAvailable();

    return 0;
}

AVPacket *QFfmpegOutputStream::takeEncodedPkt()
{
    AVPacket *pkt = Q_NULLPTR;

    if (isValid())
    {
        pkt = m_codec->takeEncodedPkt();

        if (pkt != Q_NULLPTR)
        {
            /* rescale output packet timestamp values from codec to stream timebase */
            av_packet_rescale_ts(pkt, m_codec->codecCtx()->time_base, stream()->time_base);
            pkt->stream_index = streamIndex();
        }
    }

    return pkt;
}

bool QFfmpegOutputStream::encodeFrame(QFfmpegFrame *frame)
{
    if (m_codec)
        return m_codec->encodeFrame(frame);

    return false;
}

void QFfmpegOutputStream::flush()
{
    if (m_codec)
        avcodec_flush_buffers(m_codec->codecCtx());
}

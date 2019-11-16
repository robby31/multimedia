#include "qffmpegencoder.h"

QFfmpegEncoder::QFfmpegEncoder(QObject *parent):
    QFfmpegCodec(parent)
{

}

QFfmpegEncoder::~QFfmpegEncoder()
{
    _close();
}

void QFfmpegEncoder::close()
{
    _close();
}

void QFfmpegEncoder::_close()
{
    #if !defined(QT_NO_DEBUG_OUTPUT)
    if (codecCtx() != Q_NULLPTR)
        qDebug() << format() << codecCtx()->frame_number << "frames encoded.";
    #endif

    if (!m_encodedPkt.isEmpty())
        qWarning() << m_encodedPkt.size() << "frames not encoded remains in encoder" << format();

    clear();

    QFfmpegCodec::close();
}

bool QFfmpegEncoder::encodeFrame(QFfmpegFrame *frame)
{
    bool result = false;

    if (isValid())
    {
        AVPacket *pkt = Q_NULLPTR;
        pkt = av_packet_alloc();
        if (!pkt)
        {
            qCritical() << "Could not allocate packet";
        }
        else
        {
            int ret;

            av_init_packet(pkt);

            /* send the frame to the encoder */
            if (frame != Q_NULLPTR && frame->ptr() != Q_NULLPTR)
                ret = avcodec_send_frame(codecCtx(), frame->ptr());
            else
                ret = avcodec_send_frame(codecCtx(), Q_NULLPTR); // flush encoder

            if (ret == AVERROR_EOF)
            {
                // EOF reached
            }
            else if (ret < 0)
            {
                qCritical() << "Error sending a frame for encoding";
            }
            else
            {
                while (ret >= 0)
                {
                    ret = avcodec_receive_packet(codecCtx(), pkt);
                    if (ret == 0)
                    {
                        m_encodedPkt << pkt;

                        pkt = av_packet_alloc();
                        av_init_packet(pkt);
                    }
                    else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    {
                        result = true;
                    }
                    else if (ret < 0)
                    {
                        qCritical() << "Error during encoding";
                    }

                    av_packet_unref(pkt);
                }
            }
        }

        av_packet_free(&pkt);
    }
    else
    {
        qCritical() << "codec is invalid for encoding.";
    }

    return result;
}

qint64 QFfmpegEncoder::encodedPktAvailable() const
{
    return m_encodedPkt.size();
}

AVPacket *QFfmpegEncoder::takeEncodedPkt()
{
    if (!m_encodedPkt.isEmpty())
        return m_encodedPkt.takeFirst();

    return Q_NULLPTR;
}

QByteArray QFfmpegEncoder::getRawData()
{
    QByteArray res;

    foreach (AVPacket *pkt, m_encodedPkt)
    {
        if (pkt != Q_NULLPTR)
            res.append(QByteArray::fromRawData(reinterpret_cast<char *>(pkt->data), pkt->size));
    }

    return res;
}

bool QFfmpegEncoder::setSampleFmt(const AVSampleFormat &format)
{
    if (codecCtx())
    {
        codecCtx()->sample_fmt = format;
        return true;
    }

    return false;
}

bool QFfmpegEncoder::setPixelFormat(const AVPixelFormat &format)
{
    if (codecCtx())
    {
        codecCtx()->pix_fmt = format;
        return true;
    }

    return false;
}


bool QFfmpegEncoder::setChannelLayout(const uint64_t &layout)
{
    if (codecCtx())
    {
        codecCtx()->channel_layout = layout;
        codecCtx()->channels = av_get_channel_layout_nb_channels(layout);
        return true;
    }

    return false;
}

bool QFfmpegEncoder::setChannelCount(const int &nb)
{
    if (codecCtx())
    {
        codecCtx()->channel_layout = static_cast<uint64_t>(av_get_default_channel_layout(nb));
        codecCtx()->channels = nb;
        return true;
    }

    return false;
}

bool QFfmpegEncoder::setSampleRate(const int &rate) const
{
    if (codecCtx())
    {
        codecCtx()->sample_rate = rate;
        return true;
    }

    return false;
}

bool QFfmpegEncoder::setBitRate(const qint64 &bitrate)
{
    if (codecCtx())
    {
        codecCtx()->bit_rate = bitrate;
        return true;
    }

    return false;
}

qint64 QFfmpegEncoder::nextPts() const
{
    return next_pts;
}

void QFfmpegEncoder::incrNextPts(const int &duration)
{
    next_pts += duration;
}

void QFfmpegEncoder::clear()
{
    while (!m_encodedPkt.isEmpty())
    {
        AVPacket *pkt = m_encodedPkt.takeFirst();
        av_packet_free(&pkt);
    }
}

#include "qffmpegdecoder.h"

QFfmpegDecoder::QFfmpegDecoder():
    QFfmpegCodec()
{

}

QFfmpegDecoder::~QFfmpegDecoder()
{
    close();
}

void QFfmpegDecoder::close()
{
    if (codecCtx())
        qDebug() << format() << codecCtx()->frame_number << "frames decoded.";

    if (m_decodedFrames.size() > 0)
        qWarning() << m_decodedFrames.size() << "frames not decoded remains in decoder" << format();

    clear();

    QFfmpegCodec::close();
}

void QFfmpegDecoder::clear()
{
    qDeleteAll(m_decodedFrames);
    m_decodedFrames.clear();
}

qint64 QFfmpegDecoder::decodedFramesAvailable() const
{
    return m_decodedFrames.size();
}

QFfmpegFrame *QFfmpegDecoder::takeDecodedFrame()
{
    if (!m_decodedFrames.isEmpty())
        return m_decodedFrames.takeFirst();
    else
        return Q_NULLPTR;
}

bool QFfmpegDecoder::decodePacket(AVPacket *pkt)
{
    if (isValid())
    {
        // send packet to decoder
        int ret = avcodec_send_packet(codecCtx(), pkt);
        if (ret == 0)
        {
            // decode packet to frames
            QFfmpegFrame * frame = new QFfmpegFrame();
            if (!frame->isValid())
            {
                qCritical() << "unable to create new frame";
                delete frame;
                return false;
            }
            else
            {
                int ret = 0;
                while (ret == 0)
                {
                    ret = avcodec_receive_frame(codecCtx(), frame->ptr());
                    if (ret == 0)
                    {
                        // frame decoded
                        m_decodedFrames << frame;
                        frame = new QFfmpegFrame();
                        if (!frame->isValid())
                        {
                            qCritical() << "unable to create new frame";
                            delete frame;
                            return false;
                        }
                    }
                    else
                    {
                        delete frame;
                    }

                    if (m_decodedFrames.size() > 100)
                        qDebug() << "number of decoded frames in codec is" << m_decodedFrames.size();
                }

                return true;
            }
        }
        else
        {
            qDebug() << "send packet returns" << ret;
            return false;
        }
    }
    else
    {
        if (codecCtx() == Q_NULLPTR)
            qCritical() << "invalid codec context in decodePacket";
        return false;
    }
}

#include "qffmpegdecoder.h"

QFfmpegDecoder::QFfmpegDecoder(QObject *parent):
    QFfmpegCodec(parent)
{

}

QFfmpegDecoder::~QFfmpegDecoder()
{
    _close();
}

void QFfmpegDecoder::close()
{
    _close();
}

void QFfmpegDecoder::_close()
{
    #if !defined(QT_NO_DEBUG_OUTPUT)
    if (codecCtx())
        qDebug() << format() << codecCtx()->frame_number << "frames decoded.";
    #endif

    if (!m_decodedFrames.isEmpty())
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
            auto frame = new QFfmpegFrame();
            if (!frame->isValid())
            {
                qCritical() << "unable to create new frame";
                delete frame;
                return false;
            }

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

                #if !defined(QT_NO_DEBUG_OUTPUT)
                if (m_decodedFrames.size() > 100)
                    qDebug() << "number of decoded frames in codec is" << m_decodedFrames.size();
                #endif
            }

            delete frame;
            return true;
        }

        #if !defined(QT_NO_DEBUG_OUTPUT)
        qDebug() << "send packet returns" << ret;
        #endif

        return false;
    }

    if (codecCtx() == Q_NULLPTR)
        qCritical() << "invalid codec context in decodePacket";
    return false;
}

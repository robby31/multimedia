#include "qffmpegvideoencoder.h"

QFfmpegVideoEncoder::QFfmpegVideoEncoder(QObject *parent):
    QFfmpegEncoder(parent)
{

}

QFfmpegVideoEncoder::~QFfmpegVideoEncoder()
{
    _close();
}

void QFfmpegVideoEncoder::close()
{
    _close();
}

void QFfmpegVideoEncoder::_close()
{
    if (m_rescaleCtx != Q_NULLPTR)
        sws_freeContext(m_rescaleCtx);

    QFfmpegEncoder::close();
}

bool QFfmpegVideoEncoder::init_rescale(QFfmpegCodec *input)
{
    if (input && input->codecCtx() != Q_NULLPTR && codecCtx() != Q_NULLPTR)
    {
        AVCodecContext *inputCodecCtx = input->codecCtx();
        if (inputCodecCtx->pix_fmt != AV_PIX_FMT_NONE)
            m_rescaleCtx = sws_getContext(inputCodecCtx->width, inputCodecCtx->height, inputCodecCtx->pix_fmt,
                                          codecCtx()->width, codecCtx()->height, pixelFormat(),
                                          SWS_BICUBIC, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
        if (m_rescaleCtx == Q_NULLPTR)
        {
            close();
            qCritical() << "Could not allocate rescaling context";
            return false;
        }

        return true;
    }

    close();
    return false;
}

QFfmpegFrame *QFfmpegVideoEncoder::rescaleFrame(QFfmpegFrame *frame)
{
    if (codecCtx() != Q_NULLPTR && m_rescaleCtx != Q_NULLPTR && frame != Q_NULLPTR && frame->isValid())
    {
        auto newFrame = new QFfmpegFrame();
        if (!newFrame || !newFrame->isValid() || !newFrame->init_frame(pixelFormat(), codecCtx()->width, codecCtx()->height) || !newFrame->makeWritable())
        {
            qCritical() << "Error allocation new frame.";
            delete newFrame;
            return Q_NULLPTR;
        }

        if (sws_scale(m_rescaleCtx, (const uint8_t * const *) frame->ptr()->data,
                      frame->ptr()->linesize, 0, frame->ptr()->height,
                      newFrame->ptr()->data, newFrame->ptr()->linesize) < 0)
        {
            qCritical() << "Error while rescaling";
            delete newFrame;
            return Q_NULLPTR;
        }

        return newFrame;
    }

    qCritical() << "rescale context is not initialised.";
    return Q_NULLPTR;
}

bool QFfmpegVideoEncoder::encodeFrame(QFfmpegFrame *frame)
{
    bool result = false;

    if (isValid())
    {
        if (frame == Q_NULLPTR)
        {
            // flush encoder
            return QFfmpegEncoder::encodeFrame(Q_NULLPTR);
        }

        QFfmpegFrame *inputFrame = rescaleFrame(frame);

        inputFrame->ptr()->pts = nextPts();
        incrNextPts(1);

        result = QFfmpegEncoder::encodeFrame(inputFrame);

        if (inputFrame != frame)
            delete inputFrame;
    }
    else
    {
        qCritical() << "codec is invalid for encoding.";
    }

    return result;
}

bool QFfmpegVideoEncoder::setInput(QFfmpegCodec *codec_input)
{
    // initialize rescaling for video
    if (!init_rescale(codec_input))
    {
        qCritical() << "unable to initialise video rescaling.";
        return false;
    }

    return true;
}

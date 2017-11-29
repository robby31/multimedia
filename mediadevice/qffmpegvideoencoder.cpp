#include "qffmpegvideoencoder.h"

QFfmpegVideoEncoder::QFfmpegVideoEncoder():
    QFfmpegEncoder()
{

}

QFfmpegVideoEncoder::~QFfmpegVideoEncoder()
{
    close();
}

void QFfmpegVideoEncoder::close()
{
    if (m_rescaleCtx != NULL)
        sws_freeContext(m_rescaleCtx);

    QFfmpegEncoder::close();
}

bool QFfmpegVideoEncoder::init_rescale(QFfmpegCodec *input)
{
    if (input && input->codecCtx() != NULL && codecCtx() != NULL)
    {
        AVCodecContext *inputCodecCtx = input->codecCtx();
        if (inputCodecCtx->pix_fmt != AV_PIX_FMT_NONE)
            m_rescaleCtx = sws_getContext(inputCodecCtx->width, inputCodecCtx->height, inputCodecCtx->pix_fmt,
                                          codecCtx()->width, codecCtx()->height, pixelFormat(),
                                          SWS_BICUBIC, NULL, NULL, NULL);
        if (m_rescaleCtx == NULL)
        {
            close();
            qCritical() << "Could not allocate rescaling context";
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

QFfmpegFrame *QFfmpegVideoEncoder::rescaleFrame(QFfmpegFrame *frame)
{
    if (m_rescaleCtx != NULL and frame != NULL and frame->isValid())
    {
        QFfmpegFrame *newFrame = new QFfmpegFrame();
        if (!newFrame or !newFrame->isValid())
        {
            qCritical() << "Error allocation new frame.";
            delete newFrame;
            return NULL;
        }

        newFrame->ptr()->format = pixelFormat();
        newFrame->ptr()->width = codecCtx()->width;
        newFrame->ptr()->height = codecCtx()->height;

        if (av_frame_get_buffer(newFrame->ptr(), 32) < 0)
        {
            qCritical() << "Error allocation a video buffer.";
            delete newFrame;
            return NULL;
        }

        if (av_frame_make_writable(newFrame->ptr()) < 0)
        {
            qCritical() << "unable to create new frame for resampling." << newFrame->isValid();
            delete newFrame;
            return NULL;
        }

        if (sws_scale(m_rescaleCtx, (const uint8_t * const *) frame->ptr()->data,
                      frame->ptr()->linesize, 0, frame->ptr()->height,
                      newFrame->ptr()->data, newFrame->ptr()->linesize) < 0)
        {
            qCritical() << "Error while rescaling";
            delete newFrame;
            return NULL;
        }

        return newFrame;
    }
    else
    {
        qCritical() << "rescale context is not initialised.";
        return NULL;
    }
}

bool QFfmpegVideoEncoder::encodeFrame(QFfmpegFrame *frame)
{
    bool result = false;

    if (isValid())
    {
        if (frame == NULL)
        {
            // flush encoder
            return QFfmpegEncoder::encodeFrame(NULL);
        }
        else
        {
            QFfmpegFrame *inputFrame = rescaleFrame(frame);

            inputFrame->ptr()->pts = nextPts();
            incrNextPts(1);

            result = QFfmpegEncoder::encodeFrame(inputFrame);

            if (inputFrame != frame)
                delete inputFrame;
        }
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
    else
    {
        return true;
    }
}

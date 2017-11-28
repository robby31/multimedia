#include "qffmpegframe.h"

qint64 QFfmpegFrame::objectCounter = 0;

QFfmpegFrame::QFfmpegFrame():
    m_frame(av_frame_alloc())
{
    objectCounter++;
}

QFfmpegFrame::~QFfmpegFrame()
{
    objectCounter--;

    if (isValid())
        av_frame_free(&m_frame);
}

bool QFfmpegFrame::isValid() const
{
    return m_frame != NULL;
}

AVFrame *QFfmpegFrame::ptr() const
{
    return m_frame;
}

void QFfmpegFrame::setMediaType(const AVMediaType &type)
{
    m_mediaType = type;
}

AVMediaType QFfmpegFrame::mediaType() const
{
    return m_mediaType;
}

bool QFfmpegFrame::init_frame(AVSampleFormat format, uint64_t channel_layout, int sample_rate, int nb_samples)
{
    if (m_frame != NULL)
    {
        m_frame->format = format;
        m_frame->channel_layout = channel_layout;
        m_frame->sample_rate = sample_rate;
        m_frame->nb_samples = nb_samples;

        if (av_frame_get_buffer(m_frame, 0) < 0)
        {
            qCritical() << "Error allocation frame buffer.";
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

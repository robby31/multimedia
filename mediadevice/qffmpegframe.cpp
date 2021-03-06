#include "qffmpegframe.h"

QFfmpegFrame::QFfmpegFrame(QObject *parent):
    QObject(parent),
    m_frame(av_frame_alloc())
{
    DebugInfo::add_object(this);
}

QFfmpegFrame::~QFfmpegFrame()
{
    if (isValid())
        av_frame_free(&m_frame);
}

bool QFfmpegFrame::isValid() const
{
    return m_frame != Q_NULLPTR;
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
    if (!m_frame)
        return false;

    m_frame->format = format;
    m_frame->channel_layout = channel_layout;
    m_frame->sample_rate = sample_rate;
    m_frame->nb_samples = nb_samples;

    if (av_frame_get_buffer(m_frame, 0) < 0)
    {
        qCritical() << "Error allocation frame buffer.";
        return false;
    }

    return true;
}

bool QFfmpegFrame::init_frame(AVPixelFormat format, int width, int height)
{
    if (!m_frame)
        return false;

    m_frame->format = format;
    m_frame->width = width;
    m_frame->height = height;

    if (av_frame_get_buffer(m_frame, 32) < 0)
    {
        qCritical() << "Error allocation frame buffer.";
        return false;
    }

    return true;
}

bool QFfmpegFrame::makeWritable()
{
    if (!m_frame)
        return false;

    return av_frame_make_writable(m_frame) >= 0;
}

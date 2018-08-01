#ifndef QFFMPEGFRAME_H
#define QFFMPEGFRAME_H

#include <QDebug>

extern "C" {
#include <libavutil/frame.h>
}

class QFfmpegFrame
{
public:
    QFfmpegFrame();
    virtual ~QFfmpegFrame();

    bool isValid() const;

    AVFrame *ptr() const;

    bool init_frame(AVSampleFormat format, uint64_t channels, int sample_rate, int nb_samples);
    bool init_frame(AVPixelFormat format, int width, int height);

    bool makeWritable();

    void setMediaType(const AVMediaType &type);
    AVMediaType mediaType() const;

public:
    static qint64 objectCounter;

private:
    AVFrame *m_frame = Q_NULLPTR;
    AVMediaType m_mediaType = AVMEDIA_TYPE_UNKNOWN;
};

#endif // QFFMPEGFRAME_H

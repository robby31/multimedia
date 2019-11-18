#ifndef QFFMPEGFRAME_H
#define QFFMPEGFRAME_H

#include <QDebug>
#include "debuginfo.h"

extern "C" {
#include <libavutil/frame.h>
}

class QFfmpegFrame : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(QFfmpegFrame)

public:
    explicit QFfmpegFrame(QObject *parent = Q_NULLPTR);
    ~QFfmpegFrame() Q_DECL_OVERRIDE;

    bool isValid() const;

    AVFrame *ptr() const;

    bool init_frame(AVSampleFormat format, uint64_t channel_layout, int sample_rate, int nb_samples);
    bool init_frame(AVPixelFormat format, int width, int height);

    bool makeWritable();

    void setMediaType(const AVMediaType &type);
    AVMediaType mediaType() const;

private:
    AVFrame *m_frame = Q_NULLPTR;
    AVMediaType m_mediaType = AVMEDIA_TYPE_UNKNOWN;
};

#endif // QFFMPEGFRAME_H

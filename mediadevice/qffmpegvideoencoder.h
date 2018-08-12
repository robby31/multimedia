#ifndef QFFMPEGVIDEOENCODER_H
#define QFFMPEGVIDEOENCODER_H

#include "qffmpegencoder.h"

extern "C" {
#include <libswscale/swscale.h>
}

class QFfmpegVideoEncoder : public QFfmpegEncoder
{
public:
    QFfmpegVideoEncoder() = default;
    ~QFfmpegVideoEncoder() Q_DECL_OVERRIDE;

    void close() Q_DECL_OVERRIDE;

    bool setInput(QFfmpegCodec *codec_input) Q_DECL_OVERRIDE;

    bool encodeFrame(QFfmpegFrame *frame) Q_DECL_OVERRIDE;

private:
    void _close();
    bool init_rescale(QFfmpegCodec *input);
    QFfmpegFrame *rescaleFrame(QFfmpegFrame *frame);

private:
    SwsContext *m_rescaleCtx = Q_NULLPTR;
};

#endif // QFFMPEGVIDEOENCODER_H

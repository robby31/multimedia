#ifndef QFFMPEGVIDEOENCODER_H
#define QFFMPEGVIDEOENCODER_H

#include "qffmpegencoder.h"

extern "C" {
#include <libswscale/swscale.h>
}

class QFfmpegVideoEncoder : public QFfmpegEncoder
{
public:
    QFfmpegVideoEncoder();
    virtual ~QFfmpegVideoEncoder();

    virtual void close() Q_DECL_OVERRIDE;

    virtual bool setInput(QFfmpegCodec *codec_input) Q_DECL_OVERRIDE;

    virtual bool encodeFrame(QFfmpegFrame *frame) Q_DECL_OVERRIDE;

private:
    bool init_rescale(QFfmpegCodec *input);
    QFfmpegFrame *rescaleFrame(QFfmpegFrame *frame);

private:
    SwsContext *m_rescaleCtx = NULL;
};

#endif // QFFMPEGVIDEOENCODER_H

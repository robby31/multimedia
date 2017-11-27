#ifndef QFFMPEGAUDIOENCODER_H
#define QFFMPEGAUDIOENCODER_H

#include "qffmpegencoder.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
}

class QFfmpegAudioEncoder : public QFfmpegEncoder
{
public:
    QFfmpegAudioEncoder();
    virtual ~QFfmpegAudioEncoder();

    virtual void close() Q_DECL_OVERRIDE;

    virtual bool setInput(QFfmpegCodec *codec_input) Q_DECL_OVERRIDE;

    virtual bool encodeFrame(QFfmpegFrame *frame) Q_DECL_OVERRIDE;

private:
    bool init_audio_fifo();
    bool init_resampler(QFfmpegCodec *input);

    bool add_samples_to_fifo(QFfmpegFrame *frame);
    bool encodeFrameFromFifo(const int &frame_size);

    QFfmpegFrame *resampleFrame(QFfmpegFrame *frame);

private:
    SwrContext *m_resampleCtx = NULL;
    AVAudioFifo *m_audioFifo = NULL;
};

#endif // QFFMPEGAUDIOENCODER_H

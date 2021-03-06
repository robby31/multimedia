#ifndef QFFMPEGAUDIOENCODER_H
#define QFFMPEGAUDIOENCODER_H

#include "qffmpegencoder.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
}

class QFfmpegAudioEncoder : public QFfmpegEncoder
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(QFfmpegAudioEncoder)

public:
    explicit QFfmpegAudioEncoder(QObject *parent = Q_NULLPTR);
    ~QFfmpegAudioEncoder() Q_DECL_OVERRIDE;

    void close() Q_DECL_OVERRIDE;

    bool setInput(QFfmpegCodec *codec_input) Q_DECL_OVERRIDE;

    bool encodeFrame(QFfmpegFrame *frame) Q_DECL_OVERRIDE;

private:
    void _close();

    bool init_audio_fifo();
    bool init_resampler(QFfmpegCodec *input);

    bool add_samples_to_fifo(QFfmpegFrame *frame);
    bool encodeFrameFromFifo(const int &frame_size);

    QFfmpegFrame *resampleFrame(QFfmpegFrame *frame);

private:
    SwrContext *m_resampleCtx = Q_NULLPTR;
    AVAudioFifo *m_audioFifo = Q_NULLPTR;
};

#endif // QFFMPEGAUDIOENCODER_H

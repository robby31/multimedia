#ifndef QFFMPEGDECODER_H
#define QFFMPEGDECODER_H

#include "qffmpegcodec.h"

class QFfmpegDecoder : public QFfmpegCodec
{
public:
    QFfmpegDecoder() = default;
    ~QFfmpegDecoder() Q_DECL_OVERRIDE;

    void close() Q_DECL_OVERRIDE;

    void clear();

    // decoding functions
    qint64 decodedFramesAvailable() const;
    QFfmpegFrame *takeDecodedFrame();
    bool decodePacket(AVPacket *pkt);

private:
    void _close();

private:
    QList<QFfmpegFrame*> m_decodedFrames;
};

#endif // QFFMPEGDECODER_H

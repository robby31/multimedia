#ifndef QFFMPEGDECODER_H
#define QFFMPEGDECODER_H

#include "qffmpegcodec.h"

class QFfmpegDecoder : public QFfmpegCodec
{
public:
    QFfmpegDecoder();
    virtual ~QFfmpegDecoder();

    virtual void close();

    void clear();

    // decoding functions
    qint64 decodedFramesAvailable() const;
    QFfmpegFrame *takeDecodedFrame();
    bool decodePacket(AVPacket *pkt);

private:
    QList<QFfmpegFrame*> m_decodedFrames;
};

#endif // QFFMPEGDECODER_H

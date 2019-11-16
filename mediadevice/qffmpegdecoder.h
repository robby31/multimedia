#ifndef QFFMPEGDECODER_H
#define QFFMPEGDECODER_H

#include "qffmpegcodec.h"

class QFfmpegDecoder : public QFfmpegCodec
{
    Q_OBJECT

public:
    explicit QFfmpegDecoder(QObject *parent = Q_NULLPTR);
    ~QFfmpegDecoder() Q_DECL_OVERRIDE;
    QFfmpegDecoder(QFfmpegDecoder const&) = delete;
    QFfmpegDecoder& operator =(QFfmpegDecoder const&) = delete;
    QFfmpegDecoder(QFfmpegDecoder&&) = delete;
    QFfmpegDecoder& operator=(QFfmpegDecoder&&) = delete;

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

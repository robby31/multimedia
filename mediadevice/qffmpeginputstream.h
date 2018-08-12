#ifndef QFFMPEGINPUTSTREAM_H
#define QFFMPEGINPUTSTREAM_H

#include "qffmpegstream.h"
#include "qffmpegaudiodecoder.h"
#include "qffmpegvideodecoder.h"

class QFfmpegInputStream : public QFfmpegStream
{
public:
    QFfmpegInputStream() = default;
    ~QFfmpegInputStream() Q_DECL_OVERRIDE;

    bool isValid() const Q_DECL_OVERRIDE;

    void close() Q_DECL_OVERRIDE;

    void flush() Q_DECL_OVERRIDE;

    QFfmpegDecoder *codec() const Q_DECL_OVERRIDE;

    bool init_decoding_stream(AVFormatContext *format, AVMediaType type);
    bool init_decoding_stream(AVFormatContext *format, uint index);

    // decoding functions
    qint64 decodedFramesAvailable() const;
    QFfmpegFrame *takeDecodedFrame();
    bool decodePacket(AVPacket *pkt);

private:
    void _close();

private:
    QFfmpegDecoder *m_codec = Q_NULLPTR;
};

#endif // QFFMPEGINPUTSTREAM_H

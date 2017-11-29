#ifndef QFFMPEGINPUTSTREAM_H
#define QFFMPEGINPUTSTREAM_H

#include "qffmpegstream.h"
#include "qffmpegaudiodecoder.h"
#include "qffmpegvideodecoder.h"

class QFfmpegInputStream : public QFfmpegStream
{
public:
    QFfmpegInputStream();
    virtual ~QFfmpegInputStream();

    virtual bool isValid() const Q_DECL_OVERRIDE;

    virtual void close() Q_DECL_OVERRIDE;

    virtual void flush() Q_DECL_OVERRIDE;

    virtual QFfmpegDecoder *codec() const Q_DECL_OVERRIDE;

    bool init_decoding_stream(AVFormatContext *format, AVMediaType type);
    bool init_decoding_stream(const AVCodecID id, AVFormatContext *fmtContext);

    // decoding functions
    qint64 decodedFramesAvailable() const;
    QFfmpegFrame *takeDecodedFrame();
    bool decodePacket(AVPacket *pkt);

private:
    QFfmpegDecoder *m_codec = NULL;
};

#endif // QFFMPEGINPUTSTREAM_H

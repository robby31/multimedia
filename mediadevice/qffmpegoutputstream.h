#ifndef QFFMPEGOUTPUTSTREAM_H
#define QFFMPEGOUTPUTSTREAM_H

#include "qffmpegstream.h"
#include "qffmpegaudioencoder.h"
#include "qffmpegvideoencoder.h"

class QFfmpegOutputStream : public QFfmpegStream
{
public:
    QFfmpegOutputStream();
    virtual ~QFfmpegOutputStream() Q_DECL_OVERRIDE;

    virtual bool isValid() const Q_DECL_OVERRIDE;

    virtual void close() Q_DECL_OVERRIDE;

    virtual void flush() Q_DECL_OVERRIDE;

    virtual QFfmpegEncoder *codec() const Q_DECL_OVERRIDE;

    bool init_encoding_stream(const AVCodecID id, AVFormatContext *fmtContext);

    bool openOutput();

    bool setSampleFmt(const AVSampleFormat &format);
    bool setPixelFormat(const AVPixelFormat &format);
    bool setChannelLayout(const uint64_t &layout);
    bool setSampleRate(const int &rate);
    bool setBitRate(const qint64 &bitrate);

    // encoding functions
    qint64 encodedPktAvailable() const;
    AVPacket *takeEncodedPkt();
    bool encodeFrame(QFfmpegFrame *frame);

private:
    QFfmpegEncoder *m_codec = Q_NULLPTR;
};

#endif // QFFMPEGOUTPUTSTREAM_H

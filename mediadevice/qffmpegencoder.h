#ifndef QFFMPEGENCODER_H
#define QFFMPEGENCODER_H

#include "qffmpegcodec.h"

class QFfmpegEncoder : public QFfmpegCodec
{
    Q_OBJECT

public:
    explicit QFfmpegEncoder(QObject *parent = Q_NULLPTR);
    ~QFfmpegEncoder() Q_DECL_OVERRIDE;
    QFfmpegEncoder(QFfmpegEncoder const&) = delete;
    QFfmpegEncoder& operator =(QFfmpegEncoder const&) = delete;
    QFfmpegEncoder(QFfmpegEncoder&&) = delete;
    QFfmpegEncoder& operator=(QFfmpegEncoder&&) = delete;

    void close() Q_DECL_OVERRIDE;

    virtual bool setInput(QFfmpegCodec *codec_input) = 0;

    qint64 nextPts() const;
    virtual void incrNextPts(const int &duration);

    bool setSampleFmt(const AVSampleFormat &format);
    bool setPixelFormat(const AVPixelFormat &format);
    bool setChannelLayout(const uint64_t &layout);
    bool setChannelCount(const int &nb);
    bool setSampleRate(const int &rate) const;
    bool setBitRate(const qint64 &bitrate);

    QByteArray getRawData();

    void clear();

    // encoding functions
    qint64 encodedPktAvailable() const;
    AVPacket *takeEncodedPkt();
    virtual bool encodeFrame(QFfmpegFrame *frame);

private:
    void _close();

private:
    QList<AVPacket*> m_encodedPkt;
    qint64 next_pts = 0;   // pts of the next frame that will be generated
};

#endif // QFFMPEGENCODER_H

#ifndef QFFMPEGCODEC_H
#define QFFMPEGCODEC_H

#include <QString>
#include <QDebug>
#include "qffmpegframe.h"
#include "debuginfo.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class QFfmpegCodec : public QObject
{
    Q_OBJECT

public:
    explicit QFfmpegCodec(QObject *parent = Q_NULLPTR);
    ~QFfmpegCodec() Q_DECL_OVERRIDE;
    QFfmpegCodec(QFfmpegCodec const&) = delete;
    QFfmpegCodec& operator =(QFfmpegCodec const&) = delete;
    QFfmpegCodec(QFfmpegCodec&&) = delete;
    QFfmpegCodec& operator=(QFfmpegCodec&&) = delete;

    virtual bool isValid() const;

    AVCodecContext *codecCtx() const;

    bool init_codec(AVCodec *codec, AVCodecParameters *codecpar);
    bool init_codec(const QString &codecName);
    bool init_codec(const AVCodecID &id);

    bool open();
    virtual void close();

    AVMediaType type() const;
    AVSampleFormat sampleFormat() const;
    AVPixelFormat pixelFormat() const;
    int channelCount() const;
    uint64_t channelLayout() const;
    int samplerate() const;
    qint64 bitrate() const;
    QString format() const;
    QString resolution() const;
    double frameRate() const;

    int frameNumber() const;

private:
    AVCodecContext *m_codecCtx = Q_NULLPTR;
};

#endif // QFFMPEGCODEC_H

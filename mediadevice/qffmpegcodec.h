#ifndef QFFMPEGCODEC_H
#define QFFMPEGCODEC_H

#include <QString>
#include <QDebug>
#include "qffmpegframe.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class QFfmpegCodec
{
public:
    QFfmpegCodec();
    virtual ~QFfmpegCodec();

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

public:
    static qint64 objectCounter;

private:
    AVCodecContext *m_codecCtx = NULL;
};

#endif // QFFMPEGCODEC_H

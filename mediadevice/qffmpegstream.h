#ifndef QFFMPEGSTREAM_H
#define QFFMPEGSTREAM_H

#include <QtCore>
#include <QString>
#include <QDebug>
#include "qffmpegdecoder.h"
#include "qffmpegencoder.h"

extern "C" {
#include <libavformat/avformat.h>
}

class QFfmpegStream
{

public:
    QFfmpegStream();
    virtual ~QFfmpegStream();

    virtual bool isValid() const;

    bool setStream(AVStream *stream);
    bool setStream(AVStream *stream, uint streamId);

    bool setTimeBase(int num, int den);

    QString metaData(const QString &tagName) const;

    virtual void close();
    int streamIndex() const;
    AVStream * stream() const;
    virtual QFfmpegCodec *codec() const = 0;

    virtual void flush() = 0;

    qint64 getStartTimeInMicroSec() const;
    bool setDurationInMicroSec(const qint64 &estimated_duration_Microsec);
    qint64 getDurationInMicroSec() const;
    double getDurationInSec() const;

    AVSampleFormat sampleFormat() const;

    AVPixelFormat pixelFormat() const;

    int channelCount() const;
    uint64_t channelLayout() const;

    int samplerate() const;

    qint64 bitrate() const;

    QString format() const;
    AVPacket *attached_pic() const;

    QString resolution() const;
    double frameRate() const;

    qint64 packetPosInMsec(AVPacket *pkt);
    qint64 packetNextPosInMsec(AVPacket *pkt);

private:
    void _close();

public:
    static qint64 objectCounter;

private:
    AVStream *m_stream = Q_NULLPTR;   // stream linked to AVFormatContext for decoding input
};

#endif // QFFMPEGSTREAM_H

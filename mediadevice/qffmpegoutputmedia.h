#ifndef QFFMPEGOUTPUTMEDIA_H
#define QFFMPEGOUTPUTMEDIA_H

#include "qffmpegmedia.h"
#include "qffmpeginputmedia.h"
#include "qffmpegoutputstream.h"
#include "qffmpegbuffer.h"

class QFfmpegOutputMedia : public QFfmpegMedia
{

public:
    QFfmpegOutputMedia();
    virtual ~QFfmpegOutputMedia();

    virtual AVFormatContext *context() const Q_DECL_OVERRIDE;

    virtual QFfmpegOutputStream *audioStream() const Q_DECL_OVERRIDE;
    virtual QFfmpegOutputStream *videoStream() const Q_DECL_OVERRIDE;
    virtual QFfmpegOutputStream *subtitleStream() const Q_DECL_OVERRIDE;

    virtual QString getFormat() const Q_DECL_OVERRIDE;
    virtual qint64 getDuration() const Q_DECL_OVERRIDE;
    virtual qint64 getBitrate() const Q_DECL_OVERRIDE;
    virtual qint64 size() const Q_DECL_OVERRIDE;

    void setSize(const qint64 &size);

    bool openFile(const QString &filename, QList<AVMediaType> l_mediaType);
    bool openFile(const QString &filename, const QString &formatName, QList<AVMediaType> l_mediaType);
    bool openFile(const QString &filename, const QString &formatName, QHash<AVMediaType, AVCodecID> mediaConfig);

    bool openBuffer(const QString &format, QHash<AVMediaType, AVCodecID> mediaConfig);
    virtual bool close() Q_DECL_OVERRIDE;
    bool seek_time(const qint64 &ms, const qint64 &min_ts = INT64_MIN, const qint64 &max_ts = INT64_MAX);

    QFfmpegInputMedia *inputMedia();
    bool setInputMedia(QFfmpegInputMedia *input);

    virtual bool atEnd() const Q_DECL_OVERRIDE;
    qint64 bytesAvailable() const;
    QByteArray read(const int &maxlen);

    void demuxInputToBufferSize(const int &size);

    bool encodeMedia();

    bool writeFrame(QFfmpegOutputStream *stream);

    qint64 posInMsec() const;

    virtual void setTimeStartInMsec(const qint64 &msec) Q_DECL_OVERRIDE;
    virtual qint64 timeStartInMsec() const Q_DECL_OVERRIDE;

    virtual void setTimeEndInMsec(const qint64 &msec) Q_DECL_OVERRIDE;
    virtual qint64 timeEndInMsec() const Q_DECL_OVERRIDE;

private:
    bool init_stream(QList<AVMediaType> l_mediaType);

    bool openCodec();

    bool demuxInput();

    void flush();

    qint64 headerSize() const;
    qint64 trailerSize() const;

    double overhead_factor() const;

private:
    AVFormatContext *pFormatCtx = NULL;
    bool m_headerWritten = false;

    QFfmpegOutputStream *m_audioStream = NULL;
    QFfmpegOutputStream *m_videoStream = NULL;
    QFfmpegOutputStream *m_subtitleStream = NULL;

    QFfmpegInputMedia *m_inputMedia = NULL;
    QFfmpegBuffer *m_outputBuffer = NULL;
    qint64 m_posInMsec = 0;

    qint64 m_size = -1;
};

#endif // QFFMPEGOUTPUTMEDIA_H

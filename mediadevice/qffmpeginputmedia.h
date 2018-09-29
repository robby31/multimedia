#ifndef QFFMPEGINPUTMEDIA_H
#define QFFMPEGINPUTMEDIA_H

#include "qffmpegmedia.h"
#include "qffmpeginputstream.h"
#include "qffmpegvideoencoder.h"

class QFfmpegInputMedia : public QFfmpegMedia
{

public:
    QFfmpegInputMedia() = default;
    ~QFfmpegInputMedia() Q_DECL_OVERRIDE;

    AVFormatContext *context() const Q_DECL_OVERRIDE;

    QFfmpegInputStream *getStream(const uint &index);
    QFfmpegInputStream *audioStream() const Q_DECL_OVERRIDE;
    QFfmpegInputStream *videoStream() const Q_DECL_OVERRIDE;
    QFfmpegInputStream *subtitleStream() const Q_DECL_OVERRIDE;

    bool setAudioStream(const uint &streamIndex);

    QString getFormat() const Q_DECL_OVERRIDE;
    qint64 getStartTimeInMicroSec() const Q_DECL_OVERRIDE;
    qint64 getDurationInMicroSec() const Q_DECL_OVERRIDE;
    double getDurationInSec() const Q_DECL_OVERRIDE;
    qint64 getBitrate() const Q_DECL_OVERRIDE;
    qint64 size() const Q_DECL_OVERRIDE;

    bool open(const QString &filename, const bool &flag_readPicture = false);
    bool close() Q_DECL_OVERRIDE;
    bool seek_time(const qint64 &ms, const qint64 &min_ts = INT64_MIN, const qint64 &max_ts = INT64_MAX);
    bool reset();

    bool atEnd() const Q_DECL_OVERRIDE;
    QFfmpegFrame *readFrame(const QList<AVMediaType>& l_mediaType);

    void setTimeStartInMsec(const qint64 &msec) Q_DECL_OVERRIDE;
    qint64 timeStartInMsec() const Q_DECL_OVERRIDE;

    void setTimeEndInMsec(const qint64 &msec) Q_DECL_OVERRIDE;
    qint64 timeEndInMsec() const Q_DECL_OVERRIDE;

    QByteArray getPicture() const;

private:
    bool _close();
    int readPacketFromInput(AVPacket *pkt);
    QFfmpegFrame *decodePacket(AVPacket *pkt, const QList<AVMediaType>& l_mediaType);

    void readPicture();

private:
    AVFormatContext *pFormatCtx = Q_NULLPTR;

    QFfmpegInputStream *m_audioStream = Q_NULLPTR;
    QFfmpegInputStream *m_videoStream = Q_NULLPTR;
    QFfmpegInputStream *m_subtitleStream = Q_NULLPTR;

    QByteArray m_picture;
    bool m_eof = false;
    qint64 m_timeStartMsec = -1;
    qint64 m_timeEndMsec = -1;
};

#endif // QFFMPEGINPUTMEDIA_H

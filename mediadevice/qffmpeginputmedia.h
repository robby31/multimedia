#ifndef QFFMPEGINPUTMEDIA_H
#define QFFMPEGINPUTMEDIA_H

#include "qffmpegmedia.h"
#include "qffmpeginputstream.h"
#include "qffmpegvideoencoder.h"

class QFfmpegInputMedia : public QFfmpegMedia
{

public:
    QFfmpegInputMedia();
    virtual ~QFfmpegInputMedia();

    virtual AVFormatContext *context() const Q_DECL_OVERRIDE;

    virtual QFfmpegInputStream *audioStream() const Q_DECL_OVERRIDE;
    virtual QFfmpegInputStream *videoStream() const Q_DECL_OVERRIDE;
    virtual QFfmpegInputStream *subtitleStream() const Q_DECL_OVERRIDE;

    virtual QString getFormat() const Q_DECL_OVERRIDE;
    virtual qint64 getDuration() const Q_DECL_OVERRIDE;
    virtual qint64 getBitrate() const Q_DECL_OVERRIDE;
    virtual qint64 size() const Q_DECL_OVERRIDE;

    bool open(const QString &filename, const bool &flag_readPicture = false);
    virtual bool close() Q_DECL_OVERRIDE;
    bool seek_time(const int &ms);
    bool reset();

    virtual bool atEnd() const Q_DECL_OVERRIDE;
    QFfmpegFrame *readFrame(QList<AVMediaType> l_mediaType);

    virtual void setTimeEndInMsec(const qint64 &msec) Q_DECL_OVERRIDE;
    virtual qint64 timeEndInMsec() const Q_DECL_OVERRIDE;

    QByteArray getPicture() const;

private:
    int readPacketFromInput(AVPacket *pkt);
    QFfmpegFrame *decodePacket(AVPacket *pkt, QList<AVMediaType> l_mediaType);

    void readPicture();

private:
    AVFormatContext *pFormatCtx = NULL;

    QFfmpegInputStream *m_audioStream = NULL;
    QFfmpegInputStream *m_videoStream = NULL;
    QFfmpegInputStream *m_subtitleStream = NULL;

    QByteArray m_picture;
    bool m_eof = false;
    qint64 m_timeEndMsec = -1;
};

#endif // QFFMPEGINPUTMEDIA_H

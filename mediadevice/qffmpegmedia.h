#ifndef QFFMPEGMEDIA_H
#define QFFMPEGMEDIA_H

#include <QDebug>
#include "qffmpegstream.h"

class QFfmpegMedia
{

public:
    QFfmpegMedia();
    virtual ~QFfmpegMedia();

    static QString getVersion();

    virtual AVFormatContext *context() const = 0;

    QString filename() const;

    bool isValid() const;

    void setParent(void *parent);
    void *parent() const;

    void setError(const QString &error);

    virtual bool close() = 0;
    bool isOpen();

    virtual QFfmpegStream *audioStream() const = 0;
    virtual QFfmpegStream *videoStream() const = 0;
    virtual QFfmpegStream *subtitleStream() const = 0;

    virtual qint64 size() const = 0;
    virtual QString getFormat() const = 0;
    virtual qint64 getDuration() const = 0;
    virtual qint64 getBitrate() const = 0;

    QString getAudioFormat() const;
    int getAudioChannelCount() const;
    int getAudioSamplerate() const;
    qint64 getAudioBitrate() const;

    QString getVideoResolution() const;
    double getVideoFrameRate() const;

    QHash<QString, double> getVolumeInfo(const int &timeout = 30000);

    QStringList getAudioLanguages() const;
    QStringList getVideoLanguages() const;
    QStringList getSubtitleLanguages() const;

    QString metaData(const QString &tagName) const;

    virtual bool atEnd() const = 0;

    virtual void setTimeStartInMsec(const qint64 &msec) = 0;
    virtual qint64 timeStartInMsec() const = 0;

    virtual void setTimeEndInMsec(const qint64 &msec) = 0;
    virtual qint64 timeEndInMsec() const = 0;

private:
    void init();

    QStringList getMetaDataStream(const AVMediaType &type, const QString &param) const;

public:
    static qint64 objectCounter;

private:
    void *m_parent = Q_NULLPTR;
    static bool m_initialised;
    QString m_error;
};

#endif // QFFMPEGMEDIA_H

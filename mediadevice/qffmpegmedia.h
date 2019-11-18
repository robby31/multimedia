#ifndef QFFMPEGMEDIA_H
#define QFFMPEGMEDIA_H

#include <QDebug>
#include "qffmpegstream.h"
#include "debuginfo.h"

class QFfmpegMedia : public QObject
{
    Q_OBJECT

public:
    explicit QFfmpegMedia(QObject *parent = Q_NULLPTR);

    static QString getVersion();

    virtual AVFormatContext *context() const = 0;

    QString filename() const;

    bool isValid() const;

    QString error() const;
    void setError(const QString &error);

    virtual bool close() = 0;
    bool isOpen();

    virtual QFfmpegStream *audioStream() const = 0;
    virtual QFfmpegStream *videoStream() const = 0;
    virtual QFfmpegStream *subtitleStream() const = 0;

    virtual qint64 size() const = 0;
    virtual QString getFormat() const = 0;
    virtual qint64 getStartTimeInMicroSec() const = 0;
    virtual double getDurationInSec() const = 0;
    virtual qint64 getDurationInMicroSec() const = 0;
    virtual qint64 getBitrate() const = 0;

    QString getAudioFormat() const;
    int getAudioChannelCount() const;
    int getAudioSamplerate() const;
    qint64 getAudioBitrate() const;

    QString getVideoFormat() const;
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

private:
    static bool m_initialised;
    QString m_error;
};

#endif // QFFMPEGMEDIA_H

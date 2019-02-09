#ifndef TRANSCODEDEVICE_H
#define TRANSCODEDEVICE_H

#include <QElapsedTimer>
#include <QDebug>
#include <QDateTime>
#include <QUrl>

#include "device.h"

// Format available for transcoding
enum TranscodeFormatAvailable { UNKNOWN, COPY, MP3, LPCM_S16BE, LPCM_S16LE, AAC, ALAC, WAV, MPEG2_AC3, H264_AAC, H264_AC3 };

Q_DECLARE_METATYPE(TranscodeFormatAvailable)

class TranscodeDevice : public Device
{
    Q_OBJECT

    Q_PROPERTY(TranscodeFormatAvailable format READ format WRITE setFormat NOTIFY formatChanged)

public:
    explicit TranscodeDevice(QObject *parent = Q_NULLPTR);
    ~TranscodeDevice() Q_DECL_OVERRIDE;

    void startTranscodingClock();
    qint64 transcodingElapsed();

    virtual int exitCode() const = 0;

    virtual bool waitForFinished(int msecs = 30000) = 0;

    QList<QUrl> url() const { return m_urls; }

    qint64 size() const Q_DECL_OVERRIDE;

    qint64 lengthInSeconds() const;
    qint64 lengthInMSeconds() const;

    double overheadfactor() const;

    virtual void setBitrate(const qint64 &bitrate) = 0;

    void setRange(qint64 startByte, qint64 endByte) Q_DECL_OVERRIDE;

    TranscodeFormatAvailable format() const;
    void setFormat(const TranscodeFormatAvailable &format);

    QStringList audioLanguages() const                      { return m_audioLanguages; }
    void setAudioLanguages(const QStringList &languages)    { m_audioLanguages = languages; }

    QStringList subtitleLanguages() const                   { return m_subtitleLanguages; }
    void setSubtitleLanguages(const QStringList &languages) { m_subtitleLanguages = languages; }

    QString frameRate() const                           { return m_frameRate; }
    void setFrameRate(const QString &rate)              { m_frameRate = rate; }

    int audioChannelCount() const                       { return m_audioChannelCount; }
    void setAudioChannelCount(const int &channel)       { m_audioChannelCount = channel; }

    int audioSampleRate() const                         { return m_audioSampleRate; }
    void setAudioSampleRate(const int &rate)            { m_audioSampleRate = rate; }

    void setVolumeInfo(const QHash<QString, double>& info);
    QHash<QString, double> volumeInfo() const { return m_volumeInfo; }

    qint64 pos() const  Q_DECL_OVERRIDE;

    bool open() Q_DECL_OVERRIDE;
    bool isOpen() const Q_DECL_OVERRIDE { return m_opened; }

    bool isReadyToOpen() const Q_DECL_OVERRIDE;


protected:
    qint64 fullSize() const;

private:
    void _close();
    qint64 transcodedPos() const { return pos() + bytesAvailable(); }  // position in bytes of transcoded data
    qint64 transcodedProgress() const;
    virtual double originalLengthInMSeconds() const = 0;

signals:
    void formatChanged();
    void openSignal();

public slots:
    void setUrl(const QUrl &url);
    void setUrls(const QList<QUrl> &urls);
    void urlError(const QString &message);
    void close() Q_DECL_OVERRIDE;

private slots:
    virtual void _open() = 0;

protected:
    bool m_opened = false;
    qint64 m_pos = 0;

private:
    QList<QUrl> m_urls;

    QElapsedTimer transcodeClock;

    TranscodeFormatAvailable m_format = UNKNOWN;

    QStringList m_audioLanguages;
    QStringList m_subtitleLanguages;
    QString m_frameRate;
    int m_audioChannelCount = -1;
    int m_audioSampleRate = -1;
    QHash<QString, double> m_volumeInfo;
};

#endif // TRANSCODEDEVICE_H

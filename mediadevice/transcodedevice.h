#ifndef TRANSCODEDEVICE_H
#define TRANSCODEDEVICE_H

#include <QElapsedTimer>
#include <QDebug>
#include <QDateTime>

#include "device.h"

// Format available for transcoding
enum TranscodeFormatAvailable { UNKNOWN, COPY, MP3, LPCM, AAC, ALAC, WAV, MPEG2_AC3, H264_AAC, H264_AC3 };

class TranscodeDevice : public Device
{
    Q_OBJECT

    Q_PROPERTY(TranscodeFormatAvailable format READ format WRITE setFormat NOTIFY formatChanged)

public:
    explicit TranscodeDevice(QObject *parent = 0);
    virtual ~TranscodeDevice();

    void startTranscodingClock();
    qint64 transcodingElapsed();

    virtual int exitCode() const = 0;

    virtual bool waitForFinished(int msecs = 30000) = 0;

    QString url() const { return m_url; }

    virtual qint64 size() const Q_DECL_OVERRIDE;

    qint64 lengthInSeconds() const;
    qint64 lengthInMSeconds() const;

    double overheadfactor() const;

    virtual void setBitrate(const qint64 &bitrate) = 0;

    virtual void setRange(qint64 startByte, qint64 endByte) Q_DECL_OVERRIDE;

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

    void setVolumeInfo(const QHash<QString, double> info);
    QHash<QString, double> volumeInfo() const { return m_volumeInfo; }

    virtual qint64 pos() const  Q_DECL_OVERRIDE            { return m_pos; }    // position in bytes of read data

    virtual bool open() Q_DECL_OVERRIDE;
    virtual bool isOpen() const Q_DECL_OVERRIDE { return m_opened; }

    virtual bool isReadyToOpen() const Q_DECL_OVERRIDE;


protected:
    qint64 fullSize() const;

private:
    qint64 transcodedPos() const { return pos() + bytesAvailable(); }  // position in bytes of transcoded data
    qint64 transcodedProgress() const;
    virtual qint64 originalLengthInMSeconds() const = 0;

signals:
    void formatChanged();
    void openSignal();

public slots:
    void setUrl(const QString &url);
    void urlError(const QString &message);
    virtual void close() Q_DECL_OVERRIDE;

private slots:
    virtual void _open() = 0;

protected:
    bool m_opened;
    qint64 m_pos;

private:
    QString m_url;

    QElapsedTimer transcodeClock;

    TranscodeFormatAvailable m_format;

    QStringList m_audioLanguages;
    QStringList m_subtitleLanguages;
    QString m_frameRate;
    int m_audioChannelCount;
    int m_audioSampleRate;
    QHash<QString, double> m_volumeInfo;
};

#endif // TRANSCODEDEVICE_H

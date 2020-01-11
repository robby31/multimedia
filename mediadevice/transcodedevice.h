#ifndef TRANSCODEDEVICE_H
#define TRANSCODEDEVICE_H

#include <QElapsedTimer>
#include <QDebug>
#include <QDateTime>
#include <QUrl>

#include "device.h"

// Format available for transcoding
enum TranscodeFormatAvailable { UNKNOWN, COPY, MP3, LPCM_S16BE, LPCM_S16LE, AAC, ALAC, WAV, MPEG2_AC3, H264_AAC, H264_AC3 };
enum ContainerType { DEFAULT, MPEGTS, MP4 };

Q_DECLARE_METATYPE(TranscodeFormatAvailable)

class TranscodeDevice : public Device
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(TranscodeDevice)

    Q_PROPERTY(TranscodeFormatAvailable format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(ContainerType container READ containerFormat WRITE setContainer NOTIFY containerChanged)

public:
    explicit TranscodeDevice(QObject *parent = Q_NULLPTR);
    ~TranscodeDevice() Q_DECL_OVERRIDE;

    void startTranscodingClock();
    qint64 transcodingElapsed();

    virtual int exitCode() const = 0;

    QUrl videoUrl() const;
    QUrl audioUrl() const;

    qint64 size() const Q_DECL_OVERRIDE;

    qint64 lengthInSeconds() const;
    qint64 lengthInMSeconds() const;

    double overheadfactor() const;

    virtual void setBitrate(const qint64 &bitrate) = 0;

    void setRange(qint64 startByte, qint64 endByte) Q_DECL_OVERRIDE;

    TranscodeFormatAvailable format() const;
    void setFormat(const TranscodeFormatAvailable &format);

    ContainerType containerFormat() const;
    void setContainer(const ContainerType &format);

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
    void setOpen(const bool &flag) { m_opened = flag; }

    bool isReadyToOpen() const Q_DECL_OVERRIDE;


protected:
    qint64 fullSize() const;

    qint64 _pos() const { return m_pos; }
    void _setPos(const qint64 &new_pos) { m_pos = new_pos; }

private:
    void _close();
    qint64 transcodedPos() const { return pos() + bytesAvailable(); }  // position in bytes of transcoded data
    qint64 transcodedProgress() const;
    virtual double originalLengthInMSeconds() const = 0;

signals:
    void formatChanged();
    void containerChanged();
    void openSignal();

public slots:
    void setUrl(const QUrl &url);
    void setUrls(const QUrl &videoUrl, const QUrl &audioUrl);
    void setVideoUrl(const QUrl &url);
    void setAudioUrl(const QUrl &url);
    void urlError(const QString &message);
    void close() Q_DECL_OVERRIDE;

private slots:
    virtual void _open() = 0;

private:
    QUrl m_videoUrl;
    QUrl m_audioUrl;

    bool m_opened = false;
    qint64 m_pos = 0;

    QElapsedTimer transcodeClock;

    TranscodeFormatAvailable m_format = UNKNOWN;
    ContainerType m_containerFormat = DEFAULT;

    QStringList m_audioLanguages;
    QStringList m_subtitleLanguages;
    QString m_frameRate;
    int m_audioChannelCount = -1;
    int m_audioSampleRate = -1;
    QHash<QString, double> m_volumeInfo;
};

#endif // TRANSCODEDEVICE_H

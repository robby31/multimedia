#ifndef TRANSCODEPROCESS_H
#define TRANSCODEPROCESS_H

#include <QProcess>
#include <QElapsedTimer>
#include <QDebug>
#include <QDateTime>

#include "device.h"

// Format available for transcoding
enum TranscodeFormatAvailable {UNKNOWN, COPY, MP3, LPCM, AAC, ALAC, WAV, MPEG2_AC3, H264_AAC, H264_AC3};

class TranscodeProcess : public Device
{
    Q_OBJECT

public:
    explicit TranscodeProcess(QObject *parent = 0);
    virtual ~TranscodeProcess();

    bool waitForFinished(int msecs = 30000);
    int exitCode()  const { return m_process.exitCode(); }
    bool isKilled() const { return killTranscodeProcess; }

    QString url() const { return m_url; }

    virtual qint64 size() const Q_DECL_OVERRIDE;

    qint64 lengthInSeconds() const;
    qint64 lengthInMSeconds() const;
    void setLengthInMSeconds(const qint64 duration);

    double overheadfactor() const;

    virtual void setRange(qint64 startByte, qint64 endByte) Q_DECL_OVERRIDE;

    TranscodeFormatAvailable format() const                 { return m_format; }
    void setFormat(const TranscodeFormatAvailable &format)  { m_format = format; }

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

    virtual bool atEnd() const Q_DECL_OVERRIDE;
    virtual qint64 bytesAvailable() const Q_DECL_OVERRIDE  { return m_process.bytesAvailable(); }
    virtual qint64 pos() const  Q_DECL_OVERRIDE            { return m_pos; }    // position in bytes of read data

    virtual bool open() Q_DECL_OVERRIDE;
    virtual bool isOpen() const Q_DECL_OVERRIDE { return m_opened; }

    virtual bool isReadyToOpen() const Q_DECL_OVERRIDE;

    virtual QByteArray read(qint64 maxlen) Q_DECL_OVERRIDE;

protected:
    void setProgram(const QString &program)         { m_process.setProgram(program);        }
    void setArguments(const QStringList &arguments) { m_process.setArguments(arguments);    }
    qint64 fullSize() const;

private:
    qint64 transcodedPos() const { return pos() + bytesAvailable(); }  // position in bytes of transcoded data
    qint64 transcodedProgress() const;

signals:
    void openSignal();

public slots:
    void setUrl(const QString &url);
    void urlError(const QString &message);
    virtual void close() Q_DECL_OVERRIDE;

private slots:
    void _open();
    void processStarted();
    void dataAvailable();
    void errorTrancodedData(const QProcess::ProcessError &error);
    void appendTranscodingLogMessage();
    void finishedTranscodeData(const int &exitCode, const QProcess::ExitStatus &exitStatus);
    void killProcess();

    void pause();
    void resume();


private:
    QProcess m_process;
    bool m_opened;
    QString m_url;

    qint64 m_pos;

    QElapsedTimer transcodeClock;
    bool killTranscodeProcess;  // true if the application aborts the transcoding
    bool m_paused;              // true if the transcoding has been paused

    qint64 m_durationMSecs;     // duration in mseconds
    TranscodeFormatAvailable m_format;

    QStringList m_audioLanguages;
    QStringList m_subtitleLanguages;
    QString m_frameRate;
    int m_audioChannelCount;
    int m_audioSampleRate;
    QHash<QString, double> m_volumeInfo;
};

#endif // TRANSCODEPROCESS_H

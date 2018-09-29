#ifndef TRANSCODEPROCESS_H
#define TRANSCODEPROCESS_H

#include <QProcess>

#include "transcodedevice.h"

class TranscodeProcess : public TranscodeDevice
{
    Q_OBJECT

public:
    explicit TranscodeProcess(QObject *parent = Q_NULLPTR);
    ~TranscodeProcess() Q_DECL_OVERRIDE;

    bool isKilled() const { return killTranscodeProcess; }
    bool waitForFinished(int msecs = 30000) Q_DECL_OVERRIDE;

    int exitCode() const Q_DECL_OVERRIDE { return m_process.exitCode(); }

    void setOriginalLengthInMSeconds(const double &duration);

    qint64 bitrate() const Q_DECL_OVERRIDE;  // bitrate in kbits/sec
    void setBitrate(const qint64 &bitrate) Q_DECL_OVERRIDE;

    bool atEnd() const Q_DECL_OVERRIDE;
    qint64 bytesAvailable() const Q_DECL_OVERRIDE  { return m_process.bytesAvailable(); }

    QByteArray read(qint64 maxlen) Q_DECL_OVERRIDE;

protected:
    void setProgram(const QString &program)         { m_process.setProgram(program);        }
    void setArguments(const QStringList &arguments) { m_process.setArguments(arguments);    }
    qint64 fullSize() const;

private:
    virtual void updateArguments() = 0;
    qint64 transcodedPos() const { return pos() + bytesAvailable(); }  // position in bytes of transcoded data
    qint64 transcodedProgress() const;
    double originalLengthInMSeconds() const Q_DECL_OVERRIDE;

public slots:
    void close() Q_DECL_OVERRIDE;

private slots:
    void _open() Q_DECL_OVERRIDE;
    void processStarted();
    void dataAvailable();
    void errorTrancodedData(const QProcess::ProcessError &error);
    void appendTranscodingLogMessage();
    void finishedTranscodeData(const int &exitCode, const QProcess::ExitStatus &exitStatus);
    void killProcess();

    void pause();
    void resume();


private:
    double m_durationMSecs = -1;     // original duration in mseconds without timeseekStart, timeseekEnd
    qint64 m_bitrate = -1;
    QProcess m_process;
    bool killTranscodeProcess;  // true if the application aborts the transcoding
    bool m_paused;              // true if the transcoding has been paused
};

#endif // TRANSCODEPROCESS_H

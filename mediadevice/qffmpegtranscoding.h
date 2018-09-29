#ifndef QFFMPEGTRANSCODING_H
#define QFFMPEGTRANSCODING_H

#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include "transcodedevice.h"
#include "qffmpeginputmedia.h"
#include "qffmpegoutputmedia.h"

class QFfmpegTranscoding : public TranscodeDevice
{
    Q_OBJECT

public:
    explicit QFfmpegTranscoding(QObject *parent = Q_NULLPTR);
    ~QFfmpegTranscoding() Q_DECL_OVERRIDE;

    bool waitForFinished(int msecs = 30000) Q_DECL_OVERRIDE;

    void setError(const QString &message);
    QString error() const;
    int exitCode() const Q_DECL_OVERRIDE;

    void setInput(QFfmpegInputMedia *input);
    bool isReadyToOpen() const Q_DECL_OVERRIDE;

    qint64 bitrate() const Q_DECL_OVERRIDE;  // bitrate in kbits/sec
    void setBitrate(const qint64 &bitrate) Q_DECL_OVERRIDE;
    void setAudioBitrate(const qint64 &bitrate);

    bool atEnd() const Q_DECL_OVERRIDE;
    qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    QByteArray read(qint64 maxlen) Q_DECL_OVERRIDE;

    qint64 posInMsec() const;
    qint64 progress() Q_DECL_OVERRIDE;

private:
    void _close();
    double originalLengthInMSeconds() const Q_DECL_OVERRIDE;

signals:

public slots:
    void close() Q_DECL_OVERRIDE;

private slots:
    void formatHasChanged();
    void readyForOpening();
    void _open() Q_DECL_OVERRIDE;

    void startDemux();
    void demux();

private:
    QString m_error;
    QFfmpegInputMedia *m_inputMedia = Q_NULLPTR;
    QFfmpegOutputMedia *m_outputMedia = Q_NULLPTR;
    qint64 m_nextPts = 0;

    QTimer m_timerDemux;
};

#endif // QFFMPEGTRANSCODING_H

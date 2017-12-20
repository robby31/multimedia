#include "transcodeprocess.h"

TranscodeProcess::TranscodeProcess(QObject *parent) :
    TranscodeDevice(parent),
    m_process(this),
    killTranscodeProcess(false),
    m_paused(false)
{
    connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataAvailable()));
    connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(appendTranscodingLogMessage()));
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorTrancodedData(QProcess::ProcessError)));
    connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedTranscodeData(int,QProcess::ExitStatus)));
}

TranscodeProcess::~TranscodeProcess()
{
    QString msg = QString("DESTROY TranscodeProcess, bytes available:%1, state:%2, paused?%3, durationBuffer:%4, maxBufferSize:%5").arg(bytesAvailable()).arg(m_process.state()).arg(m_paused).arg(durationBuffer()).arg(maxBufferSize());
    qDebug() << msg;

    m_process.deleteLater();
}

void TranscodeProcess::close()
{
    killProcess();
    m_process.close();
    killTranscodeProcess = false;
    TranscodeDevice::close();
}

void TranscodeProcess::_open()
{
    updateArguments();

    m_process.open(QIODevice::ReadOnly);
}

void TranscodeProcess::dataAvailable()
{
    #if !defined(QT_NO_DEBUG_OUTPUT)
    appendLog(QString("received %1 bytes transcoding data.").arg(bytesAvailable()));
    #endif

    if (!m_opened && bytesAvailable() > 0)
    {
        m_opened = true;
        emit openedSignal();
    }

    // manage buffer
//    if (m_process->state() == QProcess::Running)
//    {
//        if (bytesAvailable() > maxBufferSize() && !m_paused)
//            pause();
//    }

    if (m_opened)
        emit readyRead();
}

bool TranscodeProcess::atEnd() const
{
    if (!isOpen())
        return true;

    if (!isOpen() || bytesAvailable()>0 || m_process.state() == QProcess::Running)
        return false;
    else
        return m_process.atEnd();
}

QByteArray TranscodeProcess::read(qint64 maxlen)
{
    qint64 oldProgress = progress();

    QByteArray data;
     if (m_opened)
         data = m_process.read(maxlen);
     m_pos += data.size();

//    if (m_paused && m_process->state() != QProcess::NotRunning && bytesAvailable() < (maxBufferSize()*0.75))
//        resume();

    qint64 newProgress = progress();

    if (oldProgress != newProgress)
    {
        if (m_paused)
            emit status(QString("Transcoding paused (%1%)").arg(newProgress));
        else
            emit status(QString("Transcoding (%1%)").arg(newProgress));
    }

    return data;
}

void TranscodeProcess::appendTranscodingLogMessage()
{
    // incoming log message
    QByteArray msg(m_process.readAllStandardError());
    appendLog(msg);
}

void TranscodeProcess::errorTrancodedData(const QProcess::ProcessError &error)
{
    // trancoding failed
    if (isKilled() == false)
    {
        // an error occured
        appendLog(QString("ERROR Transcoding at %3% : error n°%2 - %1.").arg(m_process.errorString()).arg(error).arg(transcodedProgress()));
        emit errorRaised(m_process.errorString());
    }
}

void TranscodeProcess::finishedTranscodeData(const int &exitCode, const QProcess::ExitStatus &exitStatus)
{
    if (exitStatus == QProcess::NormalExit)
        appendLog(QString("TRANSCODING FINISHED with exitCode %1.").arg(exitCode));
    else
        appendLog(QString("TRANSCODING CRASHED."));
    appendLog(QString("%2% TRANSCODING DONE in %1 seconds.").arg(QTime(0, 0).addMSecs(transcodingElapsed()).toString("hh:mm:ss")).arg(transcodedProgress()));

    if (!m_opened && bytesAvailable() > 0)
    {
        m_opened = true;
        emit openedSignal();
        emit readyRead();
    }

    m_paused = false;

    if (exitStatus == QProcess::NormalExit && atEnd())
        emit endReached();

    #if !defined(QT_NO_DEBUG_OUTPUT)
    appendLog(QString("finished transcoding, %1 remaining bytes.").arg(bytesAvailable()));
    #endif

    if (isKilled() == false)
    {
        if (exitCode != 0)
        {
            // trancoding failed
            emit errorRaised(QString("Transcoding finished with status %1").arg(exitCode));
            emit status("Transcoding failed.");
        }
        else
        {
            emit status("Transcoding finished.");
        }
    }
    else
    {
        emit status("Transcoding aborted.");
    }
}

void TranscodeProcess::processStarted()
{
    qDebug() << QString("Transcoding process %1 %2").arg(m_process.program()).arg(m_process.arguments().join(' '));
    appendLog(m_process.program()+' ');
    appendLog(m_process.arguments().join(' '));

    startTranscodingClock();
}

void TranscodeProcess::killProcess()
{
    if (m_process.state() != QProcess::NotRunning) {
        appendLog(QString("KILL transcoding process."));
        killTranscodeProcess = true;
        m_process.kill();
    }
}

void TranscodeProcess::pause()
{
    qint64 pid = m_process.processId();
    if (!m_paused && m_process.state() != QProcess::NotRunning && pid > 0)
    {
        qDebug() << QString("Pause transcoding (pid: %1)").arg(pid);
        #if !defined(QT_NO_DEBUG_OUTPUT)
        appendLog(QString("PAUSE TRANSCODING"));
        #endif

        if (QProcess::startDetached(QString("kill -STOP %1").arg(pid)))
        {
            m_paused = true;
            emit status(QString("Transcoding paused (%1%)").arg(progress()));
        }
        else
        {
            qCritical() << "ERROR: unable to pause transcoding process.";
        }
    }
    else
    {
        qCritical() << "ERROR: unable to pause transcoding process.";
    }
}

void TranscodeProcess::resume()
{
    qint64 pid = m_process.processId();
    if (m_paused && m_process.state() != QProcess::NotRunning && pid > 0)
    {
        qDebug() << QString("Restart transcoding (pid: %1)").arg(pid);
        #if !defined(QT_NO_DEBUG_OUTPUT)
        appendLog(QString("RESUME TRANSCODING"));
        #endif

        if (QProcess::startDetached(QString("kill -CONT %1").arg(pid)))
        {
            m_paused = false;
            emit status(QString("Transcoding (%1%)").arg(progress()));
        }
        else
        {
            qCritical() << "ERROR: unable to resume transcoding process.";
        }
    }
    else
    {
        qCritical() << "ERROR: unable to resume transcoding process.";
    }
}

qint64 TranscodeProcess::transcodedProgress() const
{
    if (size()==0)
        return 0;
    else
        return qint64(100.0*double(transcodedPos())/double(size()));
}

bool TranscodeProcess::waitForFinished(int msecs)
{
    return m_process.waitForFinished(msecs);
}

qint64 TranscodeProcess::fullSize() const
{
    if (lengthInMSeconds() > 0 && bitrate() > 0)
    {
        if (format() == MP3)
            return (double)lengthInMSeconds()/1000.0*(double)bitrate()/8.0 + 2000;   // header size = 2000 bytes
        else
            return overheadfactor()*(double)lengthInMSeconds()/1000.0*(double)bitrate()/8.0;
    }
    else
    {
        return -1;
    }
}

qint64 TranscodeProcess::bitrate() const
{
    return m_bitrate;
}

void TranscodeProcess::setBitrate(const qint64 &bitrate)
{
    if (bitrate > 0)
    {
        m_bitrate = bitrate;
        setMaxBufferSize(m_bitrate/8*durationBuffer());
    }
    else
    {
        qCritical() << "invalid bitrate" << bitrate;
    }
}

void TranscodeProcess::setOriginalLengthInMSeconds(const qint64 duration)
{
    m_durationMSecs = duration;
}

qint64 TranscodeProcess::originalLengthInMSeconds() const
{
    return m_durationMSecs;
}

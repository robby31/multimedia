#include "transcodeprocess.h"

TranscodeProcess::TranscodeProcess(QObject *parent) :
    TranscodeDevice(parent),
    m_process(this)
{
    connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataAvailable()));
    connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(appendTranscodingLogMessage()));
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorTrancodedData(QProcess::ProcessError)));
    connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedTranscodeData(int,QProcess::ExitStatus)));
}

TranscodeProcess::~TranscodeProcess()
{
//    QString msg = QString("DESTROY TranscodeProcess, bytes available:%1, state:%2, paused?%3, durationBuffer:%4, maxBufferSize:%5").arg(bytesAvailable()).arg(m_process.state()).arg(m_paused).arg(durationBuffer()).arg(maxBufferSize());
//    qDebug() << msg;

    killProcess();
    m_process.close();
    m_process.deleteLater();
}

void TranscodeProcess::close()
{
    killProcess();
    m_process.close();
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

    if (!isOpen() && bytesAvailable() > 0)
    {
        setOpen(true);
        emit openedSignal();
    }

    // manage buffer
//    if (m_process->state() == QProcess::Running)
//    {
//        if (bytesAvailable() > maxBufferSize() && !m_paused)
//            pause();
//    }

    if (isOpen())
        emit readyRead();
}

bool TranscodeProcess::atEnd() const
{
    if (!isOpen())
        return true;

    if (!isOpen() || bytesAvailable()>0 || m_process.state() == QProcess::Running)
        return false;

    if (endByte() > 0)
        return _pos() >= endByte();

    return m_process.atEnd();
}

QByteArray TranscodeProcess::read(qint64 maxlen)
{
    qint64 oldProgress = progress();

    QByteArray data;
     if (isOpen())
         data = m_process.read(maxlen);
     _setPos(_pos() + data.size());

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

    if (startByte() > 0)
    {
        if (_pos() < startByte())
        {
#if !defined(QT_NO_DEBUG_OUTPUT)
            qDebug() << "BYTES IGNORED" << _pos() << startByte();
#endif

            return QByteArray();
        }

        if ((_pos()-data.size()) < startByte())
        {
#if !defined(QT_NO_DEBUG_OUTPUT)
            qDebug() << "REMOVE BYTES" << _pos() << startByte() << data.size() << startByte()-_pos()+data.size();
#endif

            data.remove(0, QVariant::fromValue(startByte()-_pos()).toInt()+data.size());
        }
    }

    if (endByte() > 0)
    {
        if (_pos() > endByte())
            data.chop(QVariant::fromValue(_pos()-endByte()-1).toInt());
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
    if (!isKilled())
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
    appendLog(QString("%2% TRANSCODING DONE in %1 seconds.").arg(QTime(0, 0).addMSecs(static_cast<int>(transcodingElapsed())).toString("hh:mm:ss")).arg(transcodedProgress()));

    if (!isOpen() && bytesAvailable() > 0)
    {
        setOpen(true);
        emit openedSignal();
        emit readyRead();
    }

    m_paused = false;

    if (exitStatus == QProcess::NormalExit && atEnd())
        emit endReached();

    #if !defined(QT_NO_DEBUG_OUTPUT)
    appendLog(QString("finished transcoding, %1 remaining bytes.").arg(bytesAvailable()));
    #endif

    if (!isKilled())
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
#if !defined(QT_NO_DEBUG_OUTPUT)
    qDebug() << QString("Transcoding process %1 %2").arg(m_process.program(), m_process.arguments().join(' '));
#endif

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
#if !defined(QT_NO_DEBUG_OUTPUT)
        qDebug() << QString("Pause transcoding (pid: %1)").arg(pid);
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
#if !defined(QT_NO_DEBUG_OUTPUT)
        qDebug() << QString("Restart transcoding (pid: %1)").arg(pid);
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
    if (size() == 0)
        return 0;

    return qint64(100.0*double(transcodedPos())/double(size()));
}

bool TranscodeProcess::waitForFinished(const int &timeout)
{
    return m_process.waitForFinished(timeout);
}

qint64 TranscodeProcess::fullSize() const
{
    if (lengthInMSeconds() > 0 && bitrate() > 0)
    {
        if (format() == MP3)
            return static_cast<qint64>(static_cast<double>(lengthInMSeconds())/1000.0*static_cast<double>(bitrate())/8.0) + 2000;   // header size = 2000 bytes

        return static_cast<qint64>(overheadfactor()*static_cast<double>(lengthInMSeconds())/1000.0*static_cast<double>(bitrate())/8.0);
    }

    return -1;
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

void TranscodeProcess::setOriginalLengthInMSeconds(const double& duration)
{
    m_durationMSecs = duration;
}

double TranscodeProcess::originalLengthInMSeconds() const
{
    return m_durationMSecs;
}

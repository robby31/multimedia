#include "transcodeprocess.h"

TranscodeProcess::TranscodeProcess(QObject *parent) :
    Device(parent),
    m_process(this),
    m_opened(false),
    m_url(),
    m_pos(0),
    transcodeClock(),
    killTranscodeProcess(false),
    m_paused(false),
    m_durationMSecs(-1),
    m_format(UNKNOWN),
    m_audioLanguages(),
    m_subtitleLanguages(),
    m_frameRate(""),
    m_audioChannelCount(-1),
    m_audioSampleRate(-1),
    m_volumeInfo()
{
    connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(dataAvailable()));
    connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(appendTranscodingLogMessage()));
    connect(&m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorTrancodedData(QProcess::ProcessError)));
    connect(this, SIGNAL(openSignal()), this, SLOT(_open()));
    connect(&m_process, SIGNAL(started()), this, SLOT(processStarted()));
    connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finishedTranscodeData(int,QProcess::ExitStatus)));
}

TranscodeProcess::~TranscodeProcess()
{
    QString msg = QString("DESTROY TranscodeProcess, bytes available:%1, state:%2, paused?%3, durationBuffer:%4, maxBufferSize:%5").arg(bytesAvailable()).arg(m_process.state()).arg(m_paused).arg(durationBuffer()).arg(maxBufferSize());
    qDebug() << msg;

    close();
    m_process.deleteLater();
}

void TranscodeProcess::close()
{
    killProcess();
    m_process.close();
    m_opened = false;
    m_pos = 0;
    transcodeClock.invalidate();
    killTranscodeProcess = false;
    emit closed();
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

qint64 TranscodeProcess::size() const
{
    qint64 tmp_size = fullSize();

    if (tmp_size > 0)
    {
        if (endByte() != -1)
        {
            if (startByte() != -1)
                return endByte() - startByte() + 1;
        }
        else
        {
            if (startByte() != -1 && tmp_size >= startByte())
                return tmp_size - startByte();
        }

        return tmp_size;
    }
    else
    {
        qWarning() << "length or bitrate is invalid" << lengthInMSeconds() << bitrate() << "full size = " << tmp_size << url();
        return -1;
    }
}

qint64 TranscodeProcess::lengthInSeconds() const
{
    return lengthInMSeconds()/1000;
}

qint64 TranscodeProcess::lengthInMSeconds() const
{
    if (timeSeekStart() > 0)
    {
        if (timeSeekEnd() > timeSeekStart() && (timeSeekEnd()*1000) < m_durationMSecs)
            return (timeSeekEnd() - timeSeekStart())*1000;
        else
            return m_durationMSecs - timeSeekStart()*1000;
    }
    else
    {
        if (timeSeekEnd() > 0 && (timeSeekEnd()*1000) < m_durationMSecs)
            return timeSeekEnd()*1000;
        else
            return m_durationMSecs;
    }
}

void TranscodeProcess::setLengthInMSeconds(const qint64 duration)
{
    m_durationMSecs = duration;
}

bool TranscodeProcess::atEnd() const
{
    if (!isOpen())
        return true;

    if (!isOpen() or bytesAvailable()>0 or m_process.state() == QProcess::Running)
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
    if (killTranscodeProcess == false)
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
    appendLog(QString("%2% TRANSCODING DONE in %1 seconds.").arg(QTime(0, 0).addMSecs(transcodeClock.elapsed()).toString("hh:mm:ss")).arg(transcodedProgress()));

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

    transcodeClock.start();
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

void TranscodeProcess::setVolumeInfo(const QHash<QString, double> info)
{
    m_volumeInfo = info;
}

void TranscodeProcess::setUrl(const QString &url)
{ 
    m_url = url;
    emit readyToOpen();
}

void TranscodeProcess::urlError(const QString &message)
{
    // cannot define streaming url
    emit errorRaised(message);

    emit abort();
}

bool TranscodeProcess::open()
{
    if (m_url.isEmpty())
    {
        qCritical() << "url is not defined";
        return false;
    }
    else
    {
        emit openSignal();
        return true;
    }
}

bool TranscodeProcess::isReadyToOpen() const
{
    return !m_url.isEmpty();
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

void TranscodeProcess::setRange(qint64 startByte, qint64 endByte)
{
    qint64 start = startByte;
    qint64 end = endByte;

    int tmp_size = fullSize();

    if (tmp_size > 0 && start >= tmp_size)
    {
        qWarning() << "invalid startByte (greater than file size)" << startByte << tmp_size;
        start = 0;
    }

    if (tmp_size > 0 && end >= tmp_size)
    {
        qWarning() << "invalid endByte (greater than file size)" << endByte << tmp_size;
        end = - 1;
    }

    Device::setRange(start, end);
}

double TranscodeProcess::overheadfactor() const
{
    switch (format())
    {
    case MP3:
        return 1.0;
    case LPCM:
        return 1.00001;
    case AAC:
        return 1.01;
    case ALAC:
        return 1.08;
    case H264_AAC:
    case H264_AC3:
        return 1.105;
    case MPEG2_AC3:
        return 1.0847;
    default:
        return 1.0;
    }
}

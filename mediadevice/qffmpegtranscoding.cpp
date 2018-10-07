#include "qffmpegtranscoding.h"

QFfmpegTranscoding::QFfmpegTranscoding(QObject *parent):
    TranscodeDevice(parent),
    m_timerDemux(this)
{
    connect(this, SIGNAL(formatChanged()), this, SLOT(formatHasChanged()));
    connect(this, SIGNAL(readyToOpen()), this, SLOT(readyForOpening()));

    connect(&m_timerDemux, SIGNAL(timeout()), this, SLOT(demux()));
    connect(this, SIGNAL(endReached()), &m_timerDemux, SLOT(stop()));
}

QFfmpegTranscoding::~QFfmpegTranscoding()
{
    if (m_inputMedia && m_inputMedia->parent() == this)
    {
        delete m_inputMedia;
        m_inputMedia = Q_NULLPTR;
    }

    if (m_outputMedia && m_outputMedia->parent() == this)
    {
        delete m_outputMedia;
        m_outputMedia = Q_NULLPTR;
    }

    TranscodeDevice::close();
}

bool QFfmpegTranscoding::atEnd() const
{
    if (!isOpen() || !m_outputMedia)
        return true;

    if (endByte() > 0)
        return m_pos >= endByte();

    if (m_outputMedia)
        return m_outputMedia->atEnd();

    return true;
}

qint64 QFfmpegTranscoding::bytesAvailable() const
{
    if (m_outputMedia)
        return m_outputMedia->bytesAvailable();

    return -1;
}

bool QFfmpegTranscoding::isReadyToOpen() const
{
    return !url().isEmpty() || m_inputMedia;
}

void QFfmpegTranscoding::setInput(QFfmpegInputMedia *input)
{
    if (input)
    {
        if (m_inputMedia && m_inputMedia->parent() == this)
            delete m_inputMedia;

        m_inputMedia = input;

        if (m_inputMedia->isOpen())
            emit readyToOpen();
        else
            setError("input is not open when passed to QFfmpegTranscoding");
    }
    else
    {
        setError("unable to set input");
    }
}

QByteArray QFfmpegTranscoding::read(qint64 maxlen)
{
    QElapsedTimer timer;
    timer.start();

    QString msg;
    msg = msg.append(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz"));
    msg = msg.append(QString(" read qffmpeg transcoding %1, bytes available %2 ").arg(maxlen).arg(m_outputMedia->bytesAvailable()));
    msg = msg.append("in thread ").append(QThread::currentThread()->objectName());

    QByteArray data;

    if (m_outputMedia)
    {        
        if (m_timerDemux.isActive())
        {
            if (m_outputMedia->bytesAvailable() > maxlen)
                data = m_outputMedia->read(maxlen);
            else if (m_outputMedia->bytesAvailable() > 30000)
                data = m_outputMedia->read(m_outputMedia->bytesAvailable());
            else
                data = m_outputMedia->read(30000);
        }
        else
        {
            data = m_outputMedia->read(maxlen);
        }

        m_pos += data.size();
        m_nextPts = m_outputMedia->posInMsec();

        emit status(QString("Transcoding (%1%)").arg(progress()));
    }
    else
    {
        setError("unable to read transcoding, media is not correctly initialised.");
    }

    qDebug() << msg << "ended read" << data.size() << "bytes in" << timer.elapsed() << "ms.";

    if (startByte() > 0)
    {
        if (m_pos < startByte())
        {
            qDebug() << "BYTES IGNORED" << m_pos << startByte();
            return QByteArray();
        }

        if ((m_pos-data.size()) < startByte())
        {
            qDebug() << "REMOVE BYTES" << m_pos << startByte() << data.size() << startByte()-m_pos+data.size();
            data.remove(0, QVariant::fromValue(startByte()-m_pos).toInt()+data.size());
        }
    }

    if (endByte() > 0)
    {
        if (m_pos > endByte())
            data.chop(QVariant::fromValue(m_pos-endByte()-1).toInt());
    }

    return data;
}

void QFfmpegTranscoding::readyForOpening()
{
    if (!url().isEmpty() && !m_inputMedia)
    {
        auto input = new QFfmpegInputMedia();
        input->setParent(this);
        input->open(url());
        setInput(input);
    }

    if (m_inputMedia)
    {
        if (format() == WAV || format() == LPCM_S16BE || format() == LPCM_S16LE)
        {
            if (m_inputMedia->audioStream() && m_inputMedia->audioStream()->samplerate() == 44100)
                setBitrate(1411200);
            else
                setBitrate(1536000);
        }
        else if (format() == COPY && m_outputMedia && !m_outputMedia->audioStream())
        {
            QHash<AVMediaType, AVCodecID> mediaConfig;
            mediaConfig[AVMEDIA_TYPE_AUDIO] = m_inputMedia->audioStream()->codec()->codecCtx()->codec_id;

            QString outputFormat = m_inputMedia->getFormat();
            if (outputFormat == "mov,mp4,m4a,3gp,3g2,mj2")
                outputFormat = "ipod";

            if (!m_outputMedia->openBuffer(outputFormat, mediaConfig))
            {
                setError(QString("unable to open buffer %1").arg(m_inputMedia->getFormat()));
            }
            else if (m_outputMedia->audioStream())
            {
                m_outputMedia->audioStream()->setSampleFmt(m_inputMedia->audioStream()->sampleFormat());
                setBitrate(m_inputMedia->audioStream()->bitrate());
            }
            else
            {
                setError("unable to initialize audio stream");
            }
        }

        if (!m_inputMedia->isOpen())
        {
            setError(QString("unable to open input media %1").arg(url()));
        }
    }
    else
    {
        setError("unable to open input for transcoding, media is not correctly initialised.");
    }
}

void QFfmpegTranscoding::_open()
{
    if (m_inputMedia && m_outputMedia)
    {
        if (m_inputMedia->reset() && m_outputMedia->setInputMedia(m_inputMedia))
        {
            if (startByte() <= fullSize())
            {
                double start_position = double(startByte())/double(fullSize())*double(lengthInSeconds());
                if (!m_outputMedia->seek_time(static_cast<qint64>(start_position)*1000))
                    setError(QString("unable to seek media %1").arg(start_position));
            }
            else
            {
                qWarning() << "startByte is greater than size" << startByte() << fullSize();
            }

            if (timeSeekStart() >= 0)
            {
                m_outputMedia->setTimeStartInMsec(timeSeekStart()*1000);
            }

            if (timeSeekEnd() >= 0)
            {
                m_outputMedia->setTimeEndInMsec(timeSeekEnd()*1000);
            }
            else if (endByte() >= 0)
            {
                if (size() > 0)
                {
                    m_outputMedia->setSize(size());
                }
                else
                {
                    // invalid length
                    m_outputMedia->setSize(0);
                    qWarning() << "invalid size" << size();
                }
            }

            m_opened = true;
            emit openedSignal();
        }
        else
        {
            setError("unable to set input media or seek to beginning");
        }
    }
    else
    {
        setError("unable to open transcoding, media is not correctly initialised.");
    }
}

bool QFfmpegTranscoding::waitForFinished(int msecs)
{
    QElapsedTimer timer;
    timer.start();

    while (!atEnd())
    {
        emit readyRead();

        if (msecs != -1 && timer.elapsed() > msecs)
            return false;

        QThread::msleep(200);
        QCoreApplication::processEvents();
        QThread::yieldCurrentThread();
    }

    return true;
}

qint64 QFfmpegTranscoding::posInMsec() const
{
    if (m_outputMedia)
        return m_outputMedia->posInMsec();

    return -1;
}

qint64 QFfmpegTranscoding::progress()
{
    if (lengthInMSeconds() < 0)
        return 0;

    return qRound(100.0 * (posInMsec() / 1000.0) / lengthInSeconds());
}

int QFfmpegTranscoding::exitCode() const
{
    if (m_error.isEmpty())
        return 0;

    return -1;
}

void QFfmpegTranscoding::setError(const QString &message)
{
    m_error = message;
    qCritical() << this << "ERROR" << message;
}

QString QFfmpegTranscoding::error() const
{
    return m_error;
}

void QFfmpegTranscoding::close()
{
    _close();
}

void QFfmpegTranscoding::_close()
{
    if (m_outputMedia)
    {
        // reset output media

        qint64 tmpAudioBitRate = -1;
        if (m_outputMedia->audioStream())
            tmpAudioBitRate = m_outputMedia->audioStream()->bitrate();

        delete m_outputMedia;
        m_outputMedia = Q_NULLPTR;

        setFormat(format());
        if (tmpAudioBitRate > 0)
            setAudioBitrate(tmpAudioBitRate);
    }

    m_nextPts = 0;

    TranscodeDevice::close();
}

qint64 QFfmpegTranscoding::bitrate() const
{
    if (!m_outputMedia)
       return -1;

    return m_outputMedia->getBitrate();
}

void QFfmpegTranscoding::setBitrate(const qint64 &bitrate)
{
    setAudioBitrate(bitrate);
}

void QFfmpegTranscoding::setAudioBitrate(const qint64 &bitrate)
{
    if (m_outputMedia && bitrate > 0)
    {
        if (m_outputMedia->audioStream())
            m_outputMedia->audioStream()->setBitRate(bitrate);
        else
            qCritical() << "unable to set bitrate" << bitrate << "invalid format" << format() << "or invalid audio stream" << m_outputMedia->audioStream();

        setMaxBufferSize(this->bitrate()/8*durationBuffer());
    }
    else
    {
        qCritical() << "invalid bitrate" << bitrate << "or invalid output.";
    }
}

void QFfmpegTranscoding::formatHasChanged()
{
    if (!m_outputMedia)
    {
        m_outputMedia = new QFfmpegOutputMedia();
        m_outputMedia->setParent(this);
    }

    if (m_outputMedia)
    {
        // transcode only audio
        QHash<AVMediaType, AVCodecID> mediaConfig;
        mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_NONE;

        if (format() == MP3)
        {
            if (!m_outputMedia->openBuffer("mp3", mediaConfig))
            {
                setError("unable to open buffer MP3");
            }
            else if (m_outputMedia->audioStream())
            {
                m_outputMedia->audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16P);

                //            m_outputMedia.videoStream()->setPixelFormat(AV_PIX_FMT_RGB24);
                //            m_outputMedia.videoStream()->codec()->codecCtx()->width = 300;
                //            m_outputMedia.videoStream()->codec()->codecCtx()->height = 300;
            }
            else
            {
                setError("unable to initialize audio stream");
            }
        }
        else if (format() == WAV)
        {
            if (!m_outputMedia->openBuffer("wav", mediaConfig))
            {
                setError("unable to open buffer WAV");
            }
            else if (m_outputMedia->audioStream())
            {
                m_outputMedia->audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16);

                if (m_inputMedia && m_inputMedia->audioStream() && m_inputMedia->audioStream()->samplerate() == 44100)
                    setBitrate(1411200);
                else
                    setBitrate(1536000);
            }
            else
            {
                setError("unable to initialize audio stream");
            }
        }
        else if (format() == LPCM_S16BE)
        {
            if (!m_outputMedia->openBuffer("s16be", mediaConfig))
            {
                setError("unable to open buffer LPCM");
            }
            else if (m_outputMedia->audioStream())
            {
                m_outputMedia->audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16);

                if (m_inputMedia && m_inputMedia->audioStream() && m_inputMedia->audioStream()->samplerate() == 44100)
                    setBitrate(1411200);
                else
                    setBitrate(1536000);
            }
            else
            {
                setError("unable to initialize audio stream");
            }
        }
        else if (format() == LPCM_S16LE)
        {
            if (!m_outputMedia->openBuffer("s16le", mediaConfig))
            {
                setError("unable to open buffer LPCM");
            }
            else if (m_outputMedia->audioStream())
            {
                m_outputMedia->audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16);

                if (m_inputMedia && m_inputMedia->audioStream() && m_inputMedia->audioStream()->samplerate() == 44100)
                    setBitrate(1411200);
                else
                    setBitrate(1536000);
            }
            else
            {
                setError("unable to initialize audio stream");
            }
        }
        else if (format() == AAC)
        {
            mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_AAC;

            if (!m_outputMedia->openBuffer("ipod", mediaConfig))
            {
                setError("unable to open buffer ipod");
            }
            else if (m_outputMedia->audioStream())
            {
                m_outputMedia->audioStream()->setSampleFmt(AV_SAMPLE_FMT_FLTP);

                //            m_outputMedia.videoStream()->setPixelFormat(AV_PIX_FMT_RGB24);
                //            m_outputMedia.videoStream()->codec()->codecCtx()->width = 300;
                //            m_outputMedia.videoStream()->codec()->codecCtx()->height = 300;
            }
            else
            {
                setError("unable to initialize audio stream");
            }
        }
        else if (format() == ALAC)
        {
            mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_ALAC;

            if (!m_outputMedia->openBuffer("ipod", mediaConfig))
            {
                setError("unable to open buffer ipod");
            }
            else if (m_outputMedia->audioStream())
            {
                m_outputMedia->audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16P);
                setBitrate(1411200);  // variable bitrate

                //            m_outputMedia.videoStream()->setPixelFormat(AV_PIX_FMT_RGB24);
                //            m_outputMedia.videoStream()->codec()->codecCtx()->width = 300;
                //            m_outputMedia.videoStream()->codec()->codecCtx()->height = 300;
            }
            else
            {
                setError("unable to initialize audio stream");
            }
        }
        else if (format() == COPY)
        {
            if (!m_inputMedia)
            {
                // output format will be initialized when input will be provided
            }
            else
            {
                QHash<AVMediaType, AVCodecID> mediaConfig;
                mediaConfig[AVMEDIA_TYPE_AUDIO] = m_inputMedia->audioStream()->codec()->codecCtx()->codec_id;

                QString outputFormat = m_inputMedia->getFormat();
                if (outputFormat == "mov,mp4,m4a,3gp,3g2,mj2")
                    outputFormat = "ipod";

                if (!m_outputMedia->openBuffer(outputFormat, mediaConfig))
                {
                    setError(QString("unable to open buffer %1").arg(m_inputMedia->getFormat()));
                }
                else if (m_outputMedia->audioStream())
                {
                    m_outputMedia->audioStream()->setSampleFmt(m_inputMedia->audioStream()->sampleFormat());
                    setBitrate(m_inputMedia->audioStream()->bitrate());
                }
                else
                {
                    setError("unable to initialize audio stream");
                }
            }
        }
        else
        {
            setError(QString("invalid format %1").arg(format()));
        }
    }
    else
    {
        setError("unable to set format, media is not correctly initialised.");
    }
}

double QFfmpegTranscoding::originalLengthInMSeconds() const
{
    if (m_inputMedia)
        return m_inputMedia->getDurationInMicroSec() / 1000.0;

    return 0.0;
}

void QFfmpegTranscoding::startDemux()
{
    qDebug() << QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz") << "start demux";
    m_timerDemux.start(500);
}

void QFfmpegTranscoding::demux()
{
    QString msg;
    msg = msg.append(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz"));

    if (m_outputMedia && !m_outputMedia->atEnd())
    {
        msg.append(QString(" demux qffmpeg transcoding, %1 bytes available in thread %2").arg(m_outputMedia->bytesAvailable()).arg(QThread::currentThread()->objectName()));

        QElapsedTimer timer;
        timer.start();

        qint64 prev_bytesAvailable = m_outputMedia->bytesAvailable();

        m_outputMedia->demuxInputToBufferSize(maxBufferSize());

        if (prev_bytesAvailable < m_outputMedia->bytesAvailable())
            emit readyRead();

        qDebug() << msg << "ended demux qffmpeg transcoding, bytes available" << m_outputMedia->bytesAvailable() << "in" << timer.elapsed() << "ms.";

    }
    else if (m_outputMedia && m_outputMedia->atEnd())
    {
        qDebug() << msg << "demux end reached";
        emit endReached();
        emit status("Transcoding finished.");
    }
}

#include "qffmpegoutputmedia.h"

QFfmpegOutputMedia::~QFfmpegOutputMedia()
{
    _close();
}

AVFormatContext *QFfmpegOutputMedia::context() const
{
    return pFormatCtx;
}

QString QFfmpegOutputMedia::getFormat() const
{
    if (!pFormatCtx or !pFormatCtx->oformat)
        return QString();

    return pFormatCtx->oformat->name;
}

qint64 QFfmpegOutputMedia::getDuration() const
{
    if (!m_inputMedia)
        return -1;

    return m_inputMedia->getDuration();
}

qint64 QFfmpegOutputMedia::getBitrate() const
{
    qint64 bit_rate = 0;

    if (m_audioStream && m_audioStream->isValid())
        bit_rate += m_audioStream->bitrate() * overhead_factor();

    if (m_videoStream && m_videoStream->isValid() && (!m_inputMedia or !m_inputMedia->videoStream() or m_inputMedia->videoStream()->attached_pic() == Q_NULLPTR))
        bit_rate += m_videoStream->bitrate();

    return bit_rate;
}

qint64 QFfmpegOutputMedia::size() const
{
    qint64 res = headerSize();

    if (m_audioStream && m_audioStream->isValid() && m_audioStream->getDuration() > 0 && m_audioStream->bitrate() > 0)
    {
        res += m_audioStream->getDuration() * m_audioStream->bitrate() * overhead_factor() / 8000;
    }

    if (m_videoStream && m_videoStream->isValid())
    {
        if (!m_inputMedia or !m_inputMedia->videoStream() or m_inputMedia->videoStream()->attached_pic() == Q_NULLPTR)
        {
            if (m_videoStream->getDuration() > 0 && m_videoStream->bitrate() > 0)
                res += m_videoStream->getDuration() * m_videoStream->bitrate() / 8000;
        }
        else
        {
            // attached picture
            res += m_videoStream->bitrate();
        }
    }

    res += trailerSize();

    return res;
}

bool QFfmpegOutputMedia::openFile(const QString &filename, const QList<AVMediaType> &l_mediaType)
{
    return openFile(filename, QString(), l_mediaType);
}

bool QFfmpegOutputMedia::openFile(const QString &filename, const QString &formatName, const QList<AVMediaType>& l_mediaType)
{
    QHash<AVMediaType, AVCodecID> mediaConfig;
    foreach (AVMediaType mediaType, l_mediaType)
        mediaConfig[mediaType] = AV_CODEC_ID_NONE;

    return openFile(filename, formatName, mediaConfig);
}

bool QFfmpegOutputMedia::openFile(const QString &filename, const QString &formatName, const QHash<AVMediaType, AVCodecID>& mediaConfig)
{
    // guess format based on pathname extension
    int ret = -1;
    if (formatName.isEmpty())
        ret = avformat_alloc_output_context2(&pFormatCtx, Q_NULLPTR, Q_NULLPTR, filename.toStdString().c_str());
    else
        ret = avformat_alloc_output_context2(&pFormatCtx, Q_NULLPTR, formatName.toStdString().c_str(), filename.toStdString().c_str());

    if (ret < 0 or pFormatCtx == Q_NULLPTR or pFormatCtx->oformat == Q_NULLPTR)
    {
        setError(QString("unable to open %1").arg(filename));
        return false;
    }

    for (auto it = mediaConfig.constBegin(); it != mediaConfig.constEnd(); ++it)
    {
        if (it.key() == AVMEDIA_TYPE_AUDIO && it.value() != AV_CODEC_ID_NONE)
            pFormatCtx->oformat->audio_codec = it.value();
        else if (it.key() == AVMEDIA_TYPE_VIDEO && it.value() != AV_CODEC_ID_NONE)
            pFormatCtx->oformat->video_codec = it.value();
    }

    // initialize stream
    if (!init_stream(mediaConfig.keys()))
    {
        setError(QString("unable to initialize stream in file %1").arg(filename));
        avformat_free_context(pFormatCtx);
        pFormatCtx = Q_NULLPTR;
        return false;
    }

    return true;
}

bool QFfmpegOutputMedia::openBuffer(const QString &format, const QHash<AVMediaType, AVCodecID> &mediaConfig)
{
    if (avformat_alloc_output_context2(&pFormatCtx, Q_NULLPTR, format.toStdString().c_str(), Q_NULLPTR) < 0 or pFormatCtx == Q_NULLPTR or pFormatCtx->oformat == Q_NULLPTR)
    {
        setError(QString("unable to open buffer for format %1").arg(format));
        return false;
    }

    for (auto it = mediaConfig.constBegin(); it != mediaConfig.constEnd(); ++it)
    {
        if (it.key() == AVMEDIA_TYPE_AUDIO && it.value() != AV_CODEC_ID_NONE)
            pFormatCtx->oformat->audio_codec = it.value();
        else if (it.key() == AVMEDIA_TYPE_VIDEO && it.value() != AV_CODEC_ID_NONE)
            pFormatCtx->oformat->video_codec = it.value();
    }

    // initialize stream
    if (!init_stream(mediaConfig.keys()))
    {
        setError(QString("unable to initialize stream in buffer for format %1").arg(format));
        avformat_free_context(pFormatCtx);
        pFormatCtx = Q_NULLPTR;
        return false;
    }

    return true;
}

QFfmpegInputMedia *QFfmpegOutputMedia::inputMedia()
{
    return m_inputMedia;
}

bool QFfmpegOutputMedia::setInputMedia(QFfmpegInputMedia *input)
{
    if (m_inputMedia == input)
        return true;

    if (input != Q_NULLPTR)
    {
        if (m_audioStream && input->audioStream() && input->audioStream()->isValid())
        {
            if (!m_audioStream->codec()->setInput(input->audioStream()->codec()))
                return false;
        }

        if (m_videoStream && input->videoStream() && input->videoStream()->isValid())
        {
            if (!m_videoStream->codec()->setInput(input->videoStream()->codec()))
                return false;
        }

        m_inputMedia = input;

        if (openCodec())
        {
            return true;
        }

        qCritical() << "unable to open codec";
        return false;
    }

    qCritical() << "invalid media input" << input;
    return false;
}


bool QFfmpegOutputMedia::init_stream(const QList<AVMediaType> &l_mediaType)
{
    if (l_mediaType.contains(AVMEDIA_TYPE_AUDIO) && pFormatCtx != Q_NULLPTR && pFormatCtx->oformat != Q_NULLPTR && pFormatCtx->oformat->audio_codec != AV_CODEC_ID_NONE)
    {
        m_audioStream = new QFfmpegOutputStream();
        m_audioStream->init_encoding_stream(pFormatCtx->oformat->audio_codec, pFormatCtx);
    }

    if (l_mediaType.contains(AVMEDIA_TYPE_VIDEO) && pFormatCtx != Q_NULLPTR && pFormatCtx->oformat != Q_NULLPTR && pFormatCtx->oformat->video_codec != AV_CODEC_ID_NONE)
    {
        m_videoStream = new QFfmpegOutputStream();
        m_videoStream->init_encoding_stream(pFormatCtx->oformat->video_codec, pFormatCtx);
    }

    if (m_audioStream == Q_NULLPTR && m_videoStream == Q_NULLPTR)
    {
        avformat_free_context(pFormatCtx);
        pFormatCtx = Q_NULLPTR;
        return false;
    }

    return true;
}

bool QFfmpegOutputMedia::openCodec()
{
    if (m_audioStream  && m_audioStream->isValid())
        if (!m_audioStream->openOutput())
            return false;

    if (m_videoStream && m_videoStream->isValid())
        if (!m_videoStream->openOutput())
            return false;

    // set estimated duration for encoding
    if (m_inputMedia)
    {
        if (m_audioStream)
            m_audioStream->setDuration(m_inputMedia->audioStream()->getDuration());

        if (m_videoStream)
            m_videoStream->setDuration(m_inputMedia->videoStream()->getDuration());
    }

//    av_dump_format(pFormatCtx, 0, pFormatCtx->filename, 1);

    /* open the output file, if needed */
    if (pFormatCtx != Q_NULLPTR && !(pFormatCtx->flags & AVFMT_NOFILE))
    {
        QString pathname(pFormatCtx->filename);
        if (!pathname.isEmpty())
        {
            int ret = avio_open(&pFormatCtx->pb, pFormatCtx->filename, AVIO_FLAG_WRITE);
            if (ret < 0)
            {
                qCritical() << "unable to open output" << pFormatCtx->filename;
                return false;
            }
        }
        else
        {
            m_outputBuffer = new QFfmpegBuffer();
            pFormatCtx->pb = m_outputBuffer->context();
        }
    }

    if (isOpen())
    {
        /* Write the stream header, if any. */
        int ret = avformat_write_header(pFormatCtx, Q_NULLPTR);
        if (ret < 0)
        {
            if (pFormatCtx != Q_NULLPTR)
                qCritical() << "Error occurred when writing header" << pFormatCtx->filename;
            else
                qCritical() << "Error occurred when writing header";

            return false;
        }

        m_headerWritten = true;
    }
    else
    {
        qCritical() << "Error occurred when opening output file";
        return false;
    }

    return true;
}

bool QFfmpegOutputMedia::close()
{
    return _close();
}

bool QFfmpegOutputMedia::_close()
{
    if (isOpen() && pFormatCtx != Q_NULLPTR)
    {
        QString pathname(pFormatCtx->filename);
        if (!(pFormatCtx->flags & AVFMT_NOFILE) && !pathname.isEmpty())
        {
            // write trailer when header is written and when we write in file not when we write in buffer
            if (m_headerWritten)
                av_write_trailer(pFormatCtx);

            /* Close the output file. */
            avio_closep(&pFormatCtx->pb);
        }

        m_headerWritten = false;
    }

    if (m_outputBuffer != Q_NULLPTR)
    {
        delete m_outputBuffer;
        m_outputBuffer = Q_NULLPTR;
    }

    if (m_audioStream)
    {
        delete m_audioStream;
        m_audioStream = Q_NULLPTR;
    }

    if (m_videoStream)
    {
        delete m_videoStream;
        m_videoStream = Q_NULLPTR;
    }

    if (m_subtitleStream)
    {
        delete m_subtitleStream;
        m_subtitleStream = Q_NULLPTR;
    }

    avformat_free_context(pFormatCtx);
    pFormatCtx = Q_NULLPTR;

    return true;
}

bool QFfmpegOutputMedia::seek_time(const qint64 &ms, const qint64 &min_ts, const qint64 &max_ts)
{
    if (m_inputMedia)
        return m_inputMedia->seek_time(ms, min_ts, max_ts);

    return false;
}

bool QFfmpegOutputMedia::atEnd() const
{
    if (m_size >= 0 && pFormatCtx != Q_NULLPTR && pFormatCtx->pb && pFormatCtx->pb->pos > m_size)
        return true;

    if (m_inputMedia && m_outputBuffer)
        return m_inputMedia->atEnd() && m_outputBuffer->atEnd();

    return true;
}

qint64 QFfmpegOutputMedia::bytesAvailable() const
{
    if (m_outputBuffer != Q_NULLPTR)
        return m_outputBuffer->bytesAvailable();

    return -1;
}

QByteArray QFfmpegOutputMedia::read(const qint64 &maxlen)
{
    if (m_outputBuffer != Q_NULLPTR)
    {
        while (bytesAvailable() < maxlen)
        {
            if (!demuxInput())
            {
                break;
            }
        }

        QByteArray res = m_outputBuffer->read(maxlen);

        if (atEnd())
            flush();

        return res;
    }

    qCritical() << "buffer is not initialised, unable to read it.";
    return QByteArray();
}

void QFfmpegOutputMedia::demuxInputToBufferSize(const qint64 &size)
{
    while (bytesAvailable() < size)
    {
        if (!demuxInput())
            break;
    }
}

bool QFfmpegOutputMedia::demuxInput()
{
    QList<AVMediaType> l_mediaType;
    if (m_audioStream && m_audioStream->isValid())
        l_mediaType << AVMEDIA_TYPE_AUDIO;
    if (m_videoStream && m_videoStream->isValid())
        l_mediaType << AVMEDIA_TYPE_VIDEO;

    bool frameDecoded = false;

    if (m_inputMedia != Q_NULLPTR && !m_inputMedia->atEnd())
    {
        while (!frameDecoded)
        {
            QFfmpegFrame *frame = m_inputMedia->readFrame(l_mediaType);
            if (frame == Q_NULLPTR)
                break;

            if (m_audioStream && m_audioStream->isValid() && frame->mediaType() == AVMEDIA_TYPE_AUDIO)
            {
                m_audioStream->encodeFrame(frame);
                while (m_audioStream->encodedPktAvailable() > 0)
                    writeFrame(m_audioStream);
                frameDecoded = true;
            }
            else if (m_videoStream && m_videoStream->isValid() && frame->mediaType() == AVMEDIA_TYPE_VIDEO)
            {
                m_videoStream->encodeFrame(frame);
                while (m_videoStream->encodedPktAvailable() > 0)
                    writeFrame(m_videoStream);
                frameDecoded = true;
            }

            delete frame;
        }
    }

    return frameDecoded;
}

bool QFfmpegOutputMedia::encodeMedia()
{
    if (m_audioStream && m_audioStream->isValid())
    {
        while (true)
        {
            if (!demuxInput())
                break;
        }

        flush();

        return true;
    }

    qCritical() << "invalid stream output, codec is not correctly initialised.";
    return false;
}

bool QFfmpegOutputMedia::writeFrame(QFfmpegOutputStream *stream)
{
    if (isOpen() && stream != Q_NULLPTR)
    {
        AVPacket *pkt = stream->takeEncodedPkt();

        qint64 posInMsec = stream->packetNextPosInMsec(pkt);

        /* Write the compressed frame to the media file. */
        if (av_interleaved_write_frame(pFormatCtx, pkt) < 0)
        {
            qCritical() << "unable to store encoded data";
            delete pkt;
            return false;
        }

        m_posInMsec = posInMsec;

        delete pkt;
        return true;
    }

    qCritical() << "unable to write frame in media which is not open.";
    return false;
}

qint64 QFfmpegOutputMedia::posInMsec() const
{
    return m_posInMsec;
}

void QFfmpegOutputMedia::flush()
{
    if (m_audioStream && m_audioStream->isValid())
    {
        m_audioStream->encodeFrame(Q_NULLPTR);
        while (m_audioStream->encodedPktAvailable() > 0)
            writeFrame(m_audioStream);
    }

    if (m_videoStream && m_videoStream->isValid())
    {
        m_videoStream->encodeFrame(Q_NULLPTR);
        while (m_videoStream->encodedPktAvailable() > 0)
            writeFrame(m_videoStream);
    }

    // flush muxer
    av_interleaved_write_frame(pFormatCtx, Q_NULLPTR);

    if (m_outputBuffer)
        m_outputBuffer->flush();

    if (pFormatCtx != Q_NULLPTR && pFormatCtx->pb != Q_NULLPTR)
        avio_flush(pFormatCtx->pb);
}

qint64 QFfmpegOutputMedia::timeStartInMsec() const
{
    if (m_inputMedia)
        return m_inputMedia->timeStartInMsec();

    return -1;
}

void QFfmpegOutputMedia::setTimeStartInMsec(const qint64 &msec)
{
    if (!seek_time(msec, msec))
        setError(QString("unable to seek media %1").arg(msec));
    else
        m_inputMedia->setTimeStartInMsec(msec);
}

qint64 QFfmpegOutputMedia::timeEndInMsec() const
{
    if (m_inputMedia)
        return m_inputMedia->timeEndInMsec();

    return -1;
}

void QFfmpegOutputMedia::setTimeEndInMsec(const qint64 &msec)
{
    if (m_inputMedia)
        m_inputMedia->setTimeEndInMsec(msec);
}

QFfmpegOutputStream *QFfmpegOutputMedia::audioStream() const
{
    return m_audioStream;
}

QFfmpegOutputStream *QFfmpegOutputMedia::videoStream() const
{
    return m_videoStream;
}

QFfmpegOutputStream *QFfmpegOutputMedia::subtitleStream() const
{
    return m_subtitleStream;
}

void QFfmpegOutputMedia::setSize(const qint64 &size)
{
    m_size = size;
}

qint64 QFfmpegOutputMedia::headerSize() const
{
    if (pFormatCtx != Q_NULLPTR && pFormatCtx->oformat != Q_NULLPTR)
    {
        QString formatName = QString(pFormatCtx->oformat->name);
        if (formatName == "mp3")
            return 2000;

        if (formatName == "wav")
            return 78;
    }

    return 0;
}

qint64 QFfmpegOutputMedia::trailerSize() const
{
    return 0;
}

double QFfmpegOutputMedia::overhead_factor() const
{
    QString tmpFormat = getFormat();

    if (tmpFormat == "ipod")
        return 1.03;

    if (!tmpFormat.isEmpty())
        return 1.0;

    qWarning() << "format is empty, return default overhead factor : 1.0";
    return 1.0;
}

#include "qffmpegoutputmedia.h"

QFfmpegOutputMedia::QFfmpegOutputMedia():
    QFfmpegMedia()
{

}

QFfmpegOutputMedia::~QFfmpegOutputMedia()
{
    close();
}

AVFormatContext *QFfmpegOutputMedia::context() const
{
    return pFormatCtx;
}

QString QFfmpegOutputMedia::getFormat() const
{
    if (pFormatCtx != NULL)
        return pFormatCtx->oformat->name;
    else
        return QString();
}

qint64 QFfmpegOutputMedia::getDuration() const
{
    if (m_inputMedia != NULL)
        return m_inputMedia->getDuration();
    else
        return -1;
}

qint64 QFfmpegOutputMedia::getBitrate() const
{
    qint64 bit_rate = 0;

    if (m_audioStream && m_audioStream->isValid())
        bit_rate += m_audioStream->bitrate();

    if (m_videoStream && m_videoStream->isValid() && (!m_inputMedia or !m_inputMedia->videoStream() or m_inputMedia->videoStream()->attached_pic() == NULL))
        bit_rate += m_videoStream->bitrate();

    return bit_rate;
}

qint64 QFfmpegOutputMedia::size() const
{
    qint64 duration = getDuration();
    qint64 bit_rate = getBitrate();

    if (duration != -1 && bit_rate != -1)
        return headerSize() + duration * bit_rate / 8000 + trailerSize();
    else
        return -1;
}

bool QFfmpegOutputMedia::openFile(const QString &filename, QList<AVMediaType> l_mediaType)
{
    return openFile(filename, QString(), l_mediaType);
}

bool QFfmpegOutputMedia::openFile(const QString &filename, const QString &formatName, QList<AVMediaType> l_mediaType)
{
    QHash<AVMediaType, AVCodecID> mediaConfig;
    foreach (AVMediaType mediaType, l_mediaType)
        mediaConfig[mediaType] = AV_CODEC_ID_NONE;

    return openFile(filename, formatName, mediaConfig);
}

bool QFfmpegOutputMedia::openFile(const QString &filename, const QString &formatName, QHash<AVMediaType, AVCodecID> mediaConfig)
{
    // guess format based on pathname extension
    int ret = -1;
    if (formatName.isEmpty())
        ret = avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, filename.toStdString().c_str());
    else
        ret = avformat_alloc_output_context2(&pFormatCtx, NULL, formatName.toStdString().c_str(), filename.toStdString().c_str());

    if (ret < 0 or pFormatCtx == NULL or pFormatCtx->oformat == NULL)
    {
        setError(QString("unable to open %1").arg(filename));
        return false;
    }
    else
    {
        foreach (AVMediaType mediaType, mediaConfig.keys())
        {
            if (mediaType == AVMEDIA_TYPE_AUDIO && mediaConfig[mediaType] != AV_CODEC_ID_NONE)
                pFormatCtx->oformat->audio_codec = mediaConfig[mediaType];
            else if (mediaType == AVMEDIA_TYPE_VIDEO && mediaConfig[mediaType] != AV_CODEC_ID_NONE)
                pFormatCtx->oformat->video_codec = mediaConfig[mediaType];
        }

        // initialize stream
        if (!init_stream(mediaConfig.keys()))
        {
            setError(QString("unable to initialize stream in file %1").arg(filename));
            avformat_free_context(pFormatCtx);
            pFormatCtx = NULL;
            return false;
        }
        else
        {
            return true;
        }
    }
}

bool QFfmpegOutputMedia::openBuffer(const QString &format, QHash<AVMediaType, AVCodecID> mediaConfig)
{
    if (avformat_alloc_output_context2(&pFormatCtx, NULL, format.toStdString().c_str(), NULL) < 0 or pFormatCtx == NULL or pFormatCtx->oformat == NULL)
    {
        setError(QString("unable to open buffer for format %1").arg(format));
        return false;
    }
    else
    {
        foreach (AVMediaType mediaType, mediaConfig.keys())
        {
            if (mediaType == AVMEDIA_TYPE_AUDIO && mediaConfig[mediaType] != AV_CODEC_ID_NONE)
                pFormatCtx->oformat->audio_codec = mediaConfig[mediaType];
            else if (mediaType == AVMEDIA_TYPE_VIDEO && mediaConfig[mediaType] != AV_CODEC_ID_NONE)
                pFormatCtx->oformat->video_codec = mediaConfig[mediaType];
        }

        // initialize stream
        if (!init_stream(mediaConfig.keys()))
        {
            setError(QString("unable to initialize stream in buffer for format %1").arg(format));
            avformat_free_context(pFormatCtx);
            pFormatCtx = NULL;
            return false;
        }
        else
        {
            return true;
        }
    }
}

QFfmpegInputMedia *QFfmpegOutputMedia::inputMedia()
{
    return m_inputMedia;
}

bool QFfmpegOutputMedia::setInputMedia(QFfmpegInputMedia *input)
{
    if (m_inputMedia == input)
        return true;

    if (input != NULL)
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

        if (openCodec())
        {
            m_inputMedia = input;
            return true;
        }
        else
        {
            qCritical() << "unable to open codec";
            return false;
        }
    }
    else
    {
        qCritical() << "invalid media input" << input;
        return false;
    }
}


bool QFfmpegOutputMedia::init_stream(QList<AVMediaType> l_mediaType)
{
    if (l_mediaType.contains(AVMEDIA_TYPE_AUDIO) && pFormatCtx != NULL && pFormatCtx->oformat != NULL && pFormatCtx->oformat->audio_codec != AV_CODEC_ID_NONE)
    {
        m_audioStream = new QFfmpegOutputStream();
        m_audioStream->init_encoding_stream(pFormatCtx->oformat->audio_codec, pFormatCtx);
    }

    if (l_mediaType.contains(AVMEDIA_TYPE_VIDEO) && pFormatCtx != NULL && pFormatCtx->oformat != NULL && pFormatCtx->oformat->video_codec != AV_CODEC_ID_NONE)
    {
        m_videoStream = new QFfmpegOutputStream();
        m_videoStream->init_encoding_stream(pFormatCtx->oformat->video_codec, pFormatCtx);
    }

    if (m_audioStream == NULL && m_videoStream == NULL)
    {
        avformat_free_context(pFormatCtx);
        pFormatCtx = NULL;
        return false;
    }
    else
    {
        return true;
    }
}

bool QFfmpegOutputMedia::openCodec()
{
    if (m_audioStream  && m_audioStream->isValid())
        if (!m_audioStream->openOutput())
            return false;

    if (m_videoStream && m_videoStream->isValid())
        if (!m_videoStream->openOutput())
            return false;

//    av_dump_format(pFormatCtx, 0, pFormatCtx->filename, 1);

    /* open the output file, if needed */
    if (pFormatCtx != NULL && !(pFormatCtx->flags & AVFMT_NOFILE))
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
        int ret = avformat_write_header(pFormatCtx, NULL);
        if (ret < 0)
        {
            if (pFormatCtx != NULL)
                qCritical() << "Error occurred when writing header" << pFormatCtx->filename;
            else
                qCritical() << "Error occurred when writing header";

            return false;
        }
        else
        {
            m_headerWritten = true;
        }
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
    if (isOpen() && pFormatCtx != NULL)
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

    if (m_outputBuffer != NULL)
    {
        delete m_outputBuffer;
        m_outputBuffer = NULL;
    }

    if (m_audioStream)
    {
        delete m_audioStream;
        m_audioStream = NULL;
    }

    if (m_videoStream)
    {
        delete m_videoStream;
        m_videoStream = NULL;
    }

    if (m_subtitleStream)
    {
        delete m_subtitleStream;
        m_subtitleStream = NULL;
    }

    avformat_free_context(pFormatCtx);
    pFormatCtx = NULL;

    return true;
}

bool QFfmpegOutputMedia::seek_time(const int &ms)
{
    if (m_inputMedia)
        return m_inputMedia->seek_time(ms);
    else
        return false;
}

bool QFfmpegOutputMedia::atEnd() const
{
    if (m_size >= 0 && pFormatCtx != NULL && pFormatCtx->pb && pFormatCtx->pb->pos > m_size)
        return true;

    if (m_inputMedia && m_outputBuffer)
        return m_inputMedia->atEnd() && m_outputBuffer->atEnd();
    else
        return true;
}

qint64 QFfmpegOutputMedia::bytesAvailable() const
{
    if (m_outputBuffer != NULL)
        return m_outputBuffer->bytesAvailable();
    else
        return -1;
}

QByteArray QFfmpegOutputMedia::read(const int &maxlen)
{
    if (m_outputBuffer != NULL)
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
    else
    {
        qCritical() << "buffer is not initialised, unable to read it.";
        return QByteArray();
    }
}

void QFfmpegOutputMedia::demuxInputToBufferSize(const int &size)
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

    if (m_inputMedia != NULL && !m_inputMedia->atEnd())
    {
        while (!frameDecoded)
        {
            QFfmpegFrame *frame = m_inputMedia->readFrame(l_mediaType);
            if (frame == NULL)
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
    else
    {
        qCritical() << "invalid stream output, codec is not correctly initialised.";
        return false;
    }
}

bool QFfmpegOutputMedia::writeFrame(QFfmpegOutputStream *stream)
{
    if (isOpen() && stream != NULL)
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

        m_posInMsec = posInMsec ;

        delete pkt;
        return true;
    }
    else
    {
        qCritical() << "unable to write frame in media which is not open.";
        return false;
    }
}

qint64 QFfmpegOutputMedia::posInMsec() const
{
    return m_posInMsec;
}

void QFfmpegOutputMedia::flush()
{
    if (m_audioStream && m_audioStream->isValid())
    {
        m_audioStream->encodeFrame(NULL);
        while (m_audioStream->encodedPktAvailable() > 0)
            writeFrame(m_audioStream);
    }

    if (m_videoStream && m_videoStream->isValid())
    {
        m_videoStream->encodeFrame(NULL);
        while (m_videoStream->encodedPktAvailable() > 0)
            writeFrame(m_videoStream);
    }

    // flush muxer
    av_interleaved_write_frame(pFormatCtx, NULL);

    if (m_outputBuffer)
        m_outputBuffer->flush();

    if (pFormatCtx != NULL && pFormatCtx->pb != NULL)
        avio_flush(pFormatCtx->pb);
}

qint64 QFfmpegOutputMedia::timeEndInMsec() const
{
    if (m_inputMedia)
        return m_inputMedia->timeEndInMsec();
    else
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
    if (pFormatCtx != NULL && pFormatCtx->oformat != NULL)
    {
        QString formatName = QString(pFormatCtx->oformat->name);
        if (formatName == "mp3")
            return 2000;
        else if (formatName == "wav")
            return 78;
        else
            return 0;
    }
    else
    {
        return 0;
    }
}

qint64 QFfmpegOutputMedia::trailerSize() const
{
    return 0;
}

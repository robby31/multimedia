#include "qffmpeginputmedia.h"

QFfmpegInputMedia::QFfmpegInputMedia():
    QFfmpegMedia()
{
}

QFfmpegInputMedia::~QFfmpegInputMedia()
{
    close();
}

AVFormatContext *QFfmpegInputMedia::context() const
{
    return pFormatCtx;
}

QString QFfmpegInputMedia::getFormat() const
{
    if (pFormatCtx != NULL && pFormatCtx->iformat != NULL)
        return pFormatCtx->iformat->name;
    else
        return QString();
}

qint64 QFfmpegInputMedia::getDuration() const
{
    if (pFormatCtx != NULL && pFormatCtx->duration != AV_NOPTS_VALUE)
        return 1000 * pFormatCtx->duration / AV_TIME_BASE;
    else
        return -1;
}

qint64 QFfmpegInputMedia::getBitrate() const
{
    if (pFormatCtx != NULL)
        return pFormatCtx->bit_rate;
    else
        return -1;
}

qint64 QFfmpegInputMedia::size() const
{
    qint64 size = -1;

    if (pFormatCtx != NULL && pFormatCtx->pb != NULL)
    {
        size = avio_size(pFormatCtx->pb);
        if (size >= 0)
            return size;
        else
            return -1;
    }
    else
    {
        return -1;
    }
}

bool QFfmpegInputMedia::open(const QString &filename, const bool &flag_readPicture)
{
    if (avformat_open_input(&pFormatCtx, filename.toStdString().c_str(), NULL, NULL) != 0)
    {
        close();
        setError(QString("unable to open %1").arg(filename));
        return false;
    }
    else
    {
        m_eof = false;

        if (avformat_find_stream_info(pFormatCtx, nullptr) < 0)
        {
            close();
            setError(QString("unable to open %1").arg(filename));
            return false;
        }
        else
        {
            m_audioStream = new QFfmpegInputStream();
            if (!m_audioStream->init_decoding_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO))
            {
                delete m_audioStream;
                m_audioStream = NULL;
            }

            m_videoStream = new QFfmpegInputStream();
            if (!m_videoStream->init_decoding_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO))
            {
                delete m_videoStream;
                m_videoStream = NULL;
            }

            if ((!m_audioStream or !m_audioStream->isValid()) && (!m_videoStream && !m_videoStream->isValid()))
            {
                close();
                setError(QString("unable to find stream audio or video in %1").arg(filename));
                return false;
            }
            else
            {
                if (flag_readPicture)
                    readPicture();

                return true;
            }
        }
    }
}

bool QFfmpegInputMedia::close()
{
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

    avformat_close_input(&pFormatCtx);

    return true;
}

bool QFfmpegInputMedia::reset()
{
    if (seek_time(0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool QFfmpegInputMedia::seek_time(const int &ms)
{
    if (pFormatCtx != NULL)
    {
        qint64 time = ms * 1000;
        if (avformat_seek_file(pFormatCtx, -1, INT64_MIN, time, INT64_MAX, 0) < 0)
        {
            qCritical() << "unable to seek file at position" << time / AV_TIME_BASE;
            return false;
        }
        else
        {
            m_eof = false;

            if (audioStream())
                audioStream()->flush();

            if (videoStream())
                videoStream()->flush();

            return true;
        }
    }
    else
    {
        return false;
    }
}

QFfmpegFrame *QFfmpegInputMedia::readFrame(QList<AVMediaType> l_mediaType)
{
    QFfmpegFrame *frame = NULL;

    AVPacket *pkt = av_packet_alloc();
    while (!atEnd())
    {
        int ret = readPacketFromInput(pkt);
        if (ret == AVERROR_EOF)
        {
            // flush audio decoder
            if (m_audioStream && m_audioStream->isValid() && m_audioStream->decodePacket(NULL) && m_audioStream->decodedFramesAvailable() > 0)
            {
                frame = m_audioStream->takeDecodedFrame();
                if (frame)
                    frame->setMediaType(AVMEDIA_TYPE_AUDIO);
            }

            if (frame == NULL)
            {
                // flush video decoder
                if (m_videoStream && m_videoStream->isValid() && m_videoStream->decodePacket(NULL) && m_videoStream->decodedFramesAvailable() > 0)
                {
                    frame = m_videoStream->takeDecodedFrame();
                    if (frame)
                        frame->setMediaType(AVMEDIA_TYPE_VIDEO);
                }
            }

            // no frame after flushing
            if (frame == NULL)
                m_eof = true;

            break;
        }
        else if (ret < 0)
        {
            qCritical() << "readFrame get an error" << ret;
            break;
        }
        else
        {
            // pkt read correctly
            frame = decodePacket(pkt, l_mediaType);
            if (frame != NULL)
            {
                if (m_timeEndMsec > 0)
                {
                    if (m_audioStream && pkt->stream_index == m_audioStream->streamIndex() && l_mediaType.contains(AVMEDIA_TYPE_AUDIO))
                    {
                        if (m_audioStream->packetNextPosInMsec(pkt) > m_timeEndMsec)
                        {
                            m_eof = true;
                            delete frame;
                            frame = NULL;
                        }
                    }

                    if (m_videoStream && pkt->stream_index == m_videoStream->streamIndex() && l_mediaType.contains(AVMEDIA_TYPE_VIDEO))
                    {
                        if (m_videoStream->packetNextPosInMsec(pkt) > m_timeEndMsec)
                        {
                            m_eof = true;
                            delete frame;
                            frame = NULL;
                        }
                    }
                }

                break;
            }
        }

        av_packet_unref(pkt);
    }

    av_packet_free(&pkt);

    return frame;
}

QFfmpegFrame *QFfmpegInputMedia::decodePacket(AVPacket *pkt, QList<AVMediaType> l_mediaType)
{
    QFfmpegFrame *frame = NULL;

    if (m_audioStream && pkt->stream_index == m_audioStream->streamIndex() && l_mediaType.contains(AVMEDIA_TYPE_AUDIO))
    {
        if (m_audioStream && m_audioStream->decodePacket(pkt) && m_audioStream->decodedFramesAvailable() > 0)
        {
            frame = m_audioStream->takeDecodedFrame();
            if (frame)
                frame->setMediaType(AVMEDIA_TYPE_AUDIO);
        }
    }
    else if (m_videoStream && pkt->stream_index == m_videoStream->streamIndex() && l_mediaType.contains(AVMEDIA_TYPE_VIDEO))
    {
        if (m_videoStream->decodePacket(pkt) && m_videoStream->decodedFramesAvailable() > 0)
        {
            frame = m_videoStream->takeDecodedFrame();
            if (frame)
                frame->setMediaType(AVMEDIA_TYPE_VIDEO);
        }
    }

    return frame;
}

bool QFfmpegInputMedia::atEnd() const
{
    if (pFormatCtx != NULL)
        return m_eof;
    else
        return true;
}

QByteArray QFfmpegInputMedia::getPicture() const
{
    return m_picture;
}

void QFfmpegInputMedia::readPicture()
{
    m_picture.clear();

    if (pFormatCtx != NULL && m_videoStream && m_videoStream->isValid() && m_videoStream->pixelFormat() != AV_PIX_FMT_NONE && m_videoStream->attached_pic())
    {
        QFfmpegVideoEncoder encoder;
        if (encoder.init_codec("mjpeg") && encoder.codecCtx() != NULL)
        {
            if (encoder.setPixelFormat(AV_PIX_FMT_YUVJ420P))
            {
                encoder.codecCtx()->width = 300;
                encoder.codecCtx()->height = 300;
                encoder.codecCtx()->time_base.num = 1;
                encoder.codecCtx()->time_base.den = 25;

                if (encoder.setInput(m_videoStream->codec()))
                {
                    if (encoder.open())
                    {
                        if (m_videoStream->decodePacket(m_videoStream->attached_pic()))
                        {
                            QFfmpegFrame *frame = m_videoStream->takeDecodedFrame();
                            if (frame != NULL)
                            {
                                if (encoder.encodeFrame(frame))
                                {
                                    if (encoder.encodedPktAvailable() == 1)
                                    {
                                        m_picture.append(encoder.getRawData());
                                        encoder.clear();
                                    }
                                    else
                                    {
                                        qCritical() << encoder.encodedPktAvailable() << "encoded packet available for attached picture";
                                    }
                                }
                                else
                                {
                                    qCritical() << "unable to encode frame in mjpeg";
                                }

                                delete frame;
                            }
                            else
                            {
                                qCritical() << "unable to get decoded frame for attached picture";
                            }
                        }
                        else
                        {
                            qCritical() << "unable to decode picture packet";
                        }
                    }
                    else
                    {
                        qCritical() << "unable to open encoder mjpeg";
                    }
                }
                else
                {
                    qCritical() << "unable to set input for encorder mjpeg";
                }
            }
            else
            {
                qCritical() << "unable to set pixel format AV_PIX_FMT_YUVJ420P for encoder mjpeg";
            }

        }
        else
        {
            qCritical() << "unable to init encoder mjpeg";
        }
    }
}

int QFfmpegInputMedia::readPacketFromInput(AVPacket *pkt)
{
    if (pFormatCtx != NULL)
        return av_read_frame(pFormatCtx, pkt);
    else
        return -1;
}

qint64 QFfmpegInputMedia::timeEndInMsec() const
{
    return m_timeEndMsec;
}

void QFfmpegInputMedia::setTimeEndInMsec(const qint64 &msec)
{
    m_timeEndMsec = msec;
}

QFfmpegInputStream *QFfmpegInputMedia::audioStream() const
{
    return m_audioStream;
}

QFfmpegInputStream *QFfmpegInputMedia::videoStream() const
{
    return m_videoStream;
}

QFfmpegInputStream *QFfmpegInputMedia::subtitleStream() const
{
    return m_subtitleStream;
}

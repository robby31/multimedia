#include "qffmpeginputmedia.h"

QFfmpegInputMedia::~QFfmpegInputMedia()
{
    _close();
}

AVFormatContext *QFfmpegInputMedia::context() const
{
    return pFormatCtx;
}

QString QFfmpegInputMedia::getFormat() const
{
    if (pFormatCtx != Q_NULLPTR && pFormatCtx->iformat != Q_NULLPTR)
        return pFormatCtx->iformat->name;

    return QString();
}

qint64 QFfmpegInputMedia::getStartTimeInMicroSec() const
{
    if (pFormatCtx != Q_NULLPTR && pFormatCtx->start_time != AV_NOPTS_VALUE)
        return pFormatCtx->start_time * 1000000 / AV_TIME_BASE;

    return 0.0;
}

double QFfmpegInputMedia::getDurationInSec() const
{
    return QVariant::fromValue(getDurationInMicroSec()).toDouble() / 1000000.0;
}

qint64 QFfmpegInputMedia::getDurationInMicroSec() const
{
    if (pFormatCtx != Q_NULLPTR)
        if (pFormatCtx->duration != AV_NOPTS_VALUE)

            return pFormatCtx->duration * 1000000 / AV_TIME_BASE;

    return 0.0;
}

qint64 QFfmpegInputMedia::getBitrate() const
{
    if (pFormatCtx != Q_NULLPTR)
        return pFormatCtx->bit_rate;

    return -1;
}

qint64 QFfmpegInputMedia::size() const
{
    if (pFormatCtx && pFormatCtx->pb)
    {
        qint64 size = -1;
        size = avio_size(pFormatCtx->pb);
        if (size >= 0)
            return size;
    }

    return -1;
}

QFfmpegInputStream *QFfmpegInputMedia::getStream(const uint &index)
{
    if (index < pFormatCtx->nb_streams)
    {
        auto stream = new QFfmpegInputStream();
        if (stream->init_decoding_stream(pFormatCtx, index))
            return stream;
    }

    return Q_NULLPTR;
}

bool QFfmpegInputMedia::setAudioStream(const uint &streamIndex)
{
    if (streamIndex < pFormatCtx->nb_streams)
    {
        AVStream *stream = pFormatCtx->streams[streamIndex];
        if (stream && stream->codecpar && stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            if (m_audioStream)
            {
                delete m_audioStream;
                m_audioStream = Q_NULLPTR;
            }

            m_audioStream = new QFfmpegInputStream();
            if (!m_audioStream->init_decoding_stream(pFormatCtx, streamIndex))
            {
                delete m_audioStream;
                m_audioStream = Q_NULLPTR;
                return false;
            }

            return true;
        }

        return false;
    }

    return false;
}

bool QFfmpegInputMedia::open(const QString &filename, const bool &flag_readPicture)
{
    if (avformat_open_input(&pFormatCtx, filename.toStdString().c_str(), Q_NULLPTR, Q_NULLPTR) != 0)
    {
        close();
        setError(QString("unable to open %1").arg(filename));
        return false;
    }

    m_eof = false;

    if (avformat_find_stream_info(pFormatCtx, Q_NULLPTR) < 0)
    {
        close();
        setError(QString("unable to open %1").arg(filename));
        return false;
    }

    m_audioStream = new QFfmpegInputStream();
    if (!m_audioStream->init_decoding_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO))
    {
        delete m_audioStream;
        m_audioStream = Q_NULLPTR;
    }

    m_videoStream = new QFfmpegInputStream();
    if (!m_videoStream->init_decoding_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO))
    {
        delete m_videoStream;
        m_videoStream = Q_NULLPTR;
    }

    if ((!m_audioStream || !m_audioStream->isValid()) && (!m_videoStream || !m_videoStream->isValid()))
    {
        close();
        setError(QString("unable to find stream audio or video in %1").arg(filename));
        return false;
    }

    if (flag_readPicture)
        readPicture();

    return true;
}

bool QFfmpegInputMedia::close()
{
    return _close();
}

bool QFfmpegInputMedia::_close()
{
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

    avformat_close_input(&pFormatCtx);

    return true;
}

bool QFfmpegInputMedia::reset()
{
    return seek_time(0);
}

bool QFfmpegInputMedia::seek_time(const qint64 &ms, const qint64 &min_ts, const qint64 &max_ts)
{
    if (pFormatCtx != Q_NULLPTR)
    {
        qint64 time = ms * 1000;
        if (avformat_seek_file(pFormatCtx, -1, min_ts, time, max_ts, 0) < 0)
        {
            qCritical() << "unable to seek file at position" << time / AV_TIME_BASE;
            return false;
        }

        m_eof = false;

        if (audioStream())
            audioStream()->flush();

        if (videoStream())
            videoStream()->flush();

        return true;
    }

    return false;
}

QFfmpegFrame *QFfmpegInputMedia::readFrame(const QList<AVMediaType> &l_mediaType)
{
    QFfmpegFrame *frame = Q_NULLPTR;

    AVPacket *pkt = av_packet_alloc();
    while (!atEnd())
    {
        int ret = readPacketFromInput(pkt);
        if (ret == AVERROR_EOF)
        {
            // flush audio decoder
            if (m_audioStream && m_audioStream->isValid() && m_audioStream->decodePacket(Q_NULLPTR) && m_audioStream->decodedFramesAvailable() > 0)
            {
                frame = m_audioStream->takeDecodedFrame();
                if (frame)
                    frame->setMediaType(AVMEDIA_TYPE_AUDIO);
            }

            if (frame == Q_NULLPTR)
            {
                // flush video decoder
                if (m_videoStream && m_videoStream->isValid() && m_videoStream->decodePacket(Q_NULLPTR) && m_videoStream->decodedFramesAvailable() > 0)
                {
                    frame = m_videoStream->takeDecodedFrame();
                    if (frame)
                        frame->setMediaType(AVMEDIA_TYPE_VIDEO);
                }
            }

            // no frame after flushing
            if (frame == Q_NULLPTR)
                m_eof = true;

            break;
        }

        if (ret < 0)
        {
            qCritical() << "readFrame get an error" << ret;
            break;
        }

        // pkt read correctly
        frame = decodePacket(pkt, l_mediaType);
        if (frame != Q_NULLPTR)
        {
            if (m_timeEndMsec > 0)
            {
                if (m_audioStream && pkt->stream_index == m_audioStream->streamIndex() && l_mediaType.contains(AVMEDIA_TYPE_AUDIO))
                {
                    if (m_audioStream->packetNextPosInMsec(pkt) > m_timeEndMsec)
                    {
                        m_eof = true;
                        delete frame;
                        frame = Q_NULLPTR;
                    }
                }

                if (m_videoStream && pkt->stream_index == m_videoStream->streamIndex() && l_mediaType.contains(AVMEDIA_TYPE_VIDEO))
                {
                    if (m_videoStream->packetNextPosInMsec(pkt) > m_timeEndMsec)
                    {
                        m_eof = true;
                        delete frame;
                        frame = Q_NULLPTR;
                    }
                }
            }

            break;
        }

        av_packet_unref(pkt);
    }

    av_packet_free(&pkt);

    return frame;
}

QFfmpegFrame *QFfmpegInputMedia::decodePacket(AVPacket *pkt, const QList<AVMediaType> &l_mediaType)
{
    QFfmpegFrame *frame = Q_NULLPTR;

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
    if (pFormatCtx != Q_NULLPTR)
        return m_eof;

    return true;
}

QByteArray QFfmpegInputMedia::getPicture() const
{
    return m_picture;
}

void QFfmpegInputMedia::readPicture()
{
    m_picture.clear();

    if (pFormatCtx != Q_NULLPTR && m_videoStream && m_videoStream->isValid() && m_videoStream->pixelFormat() != AV_PIX_FMT_NONE && m_videoStream->attached_pic())
    {
        QFfmpegVideoEncoder encoder;
        if (encoder.init_codec("mjpeg") && encoder.codecCtx() != Q_NULLPTR)
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
                            if (frame != Q_NULLPTR)
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
    if (pFormatCtx)
        return av_read_frame(pFormatCtx, pkt);

    return -1;
}

qint64 QFfmpegInputMedia::timeStartInMsec() const
{
    return m_timeStartMsec;
}

void QFfmpegInputMedia::setTimeStartInMsec(const qint64 &msec)
{
    m_timeStartMsec = msec;
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

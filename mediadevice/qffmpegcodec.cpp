#include "qffmpegcodec.h"

qint64 QFfmpegCodec::objectCounter = 0;

QFfmpegCodec::QFfmpegCodec()
{
    objectCounter++;
}

QFfmpegCodec::~QFfmpegCodec()
{
    objectCounter--;
}

void QFfmpegCodec::close()
{
    if (m_codecCtx != Q_NULLPTR)
        avcodec_free_context(&m_codecCtx);
}

bool QFfmpegCodec::isValid() const
{
    return m_codecCtx != Q_NULLPTR;
}

AVCodecContext *QFfmpegCodec::codecCtx() const
{
    return m_codecCtx;
}

bool QFfmpegCodec::init_codec(AVCodec *codec, AVCodecParameters *codecpar)
{
    if (codec != Q_NULLPTR && codecpar != Q_NULLPTR)
    {
        m_codecCtx = avcodec_alloc_context3(codec);
        if (m_codecCtx == Q_NULLPTR or avcodec_parameters_to_context(m_codecCtx, codecpar) != 0)
        {
            qCritical() << "unable to init codec";
            close();
            return false; // Error copying codec context
        }
        else
        {
            // open codec
            return open();
        }
    }
    else
    {
        qCritical() << "invalid parameters" << codec << codecpar;
        return false;
    }
}

bool QFfmpegCodec::init_codec(const QString &codecName)
{
    AVCodec *codec = avcodec_find_encoder_by_name(codecName.toUtf8());
    if (codec != Q_NULLPTR)
    {
        m_codecCtx = avcodec_alloc_context3(codec);
        if (m_codecCtx == Q_NULLPTR)
        {
            qCritical() << "unable to init codec";
            close();
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        qCritical() << "unable to find codec" << codecName;
        return false;
    }
}

bool QFfmpegCodec::init_codec(const AVCodecID &id)
{
    AVCodec *codec = avcodec_find_encoder(id);
    if (codec != Q_NULLPTR)
    {
        m_codecCtx = avcodec_alloc_context3(codec);
        if (m_codecCtx == Q_NULLPTR)
        {
            qCritical() << "unable to init codec";
            close();
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        qCritical() << "unable to find codec" << id;
        return false;
    }
}

bool QFfmpegCodec::open()
{
    if (isValid())
    {
        if (!avcodec_is_open(m_codecCtx))
        {
            if (avcodec_open2(m_codecCtx, Q_NULLPTR, nullptr) < 0)
            {
                qCritical() << "unable to open codec";
                close();
                return false; // Could not open codec
            }
            else
            {
                return true;
            }
        }
        else
        {
            // codec already open
            qWarning() << "codec already open";
            return true;
        }
    }
    else
    {
        close();
        return false;
    }
}

AVMediaType QFfmpegCodec::type() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->codec_type;
    else
        return AVMEDIA_TYPE_UNKNOWN;
}

AVSampleFormat QFfmpegCodec::sampleFormat() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->sample_fmt;
    else
        return AV_SAMPLE_FMT_NONE;
}

AVPixelFormat QFfmpegCodec::pixelFormat() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->pix_fmt;
    else
        return AV_PIX_FMT_NONE;
}

int QFfmpegCodec::channelCount() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->channels;
    else
        return -1;
}

uint64_t QFfmpegCodec::channelLayout() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->channel_layout;
    else
        return 0;
}

int QFfmpegCodec::samplerate() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->sample_rate;
    else
        return -1;
}

qint64 QFfmpegCodec::bitrate() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->bit_rate;
    else
        return -1;
}

QString QFfmpegCodec::format() const
{
    if (m_codecCtx != Q_NULLPTR && m_codecCtx->codec_descriptor != Q_NULLPTR)
        return m_codecCtx->codec_descriptor->name;
    else
        return QString();
}

QString QFfmpegCodec::resolution() const
{
    if (m_codecCtx != Q_NULLPTR)
        return QString("%1x%2").arg(m_codecCtx->width).arg(m_codecCtx->height);
    else
        return QString();
}

double QFfmpegCodec::frameRate() const
{
    if (m_codecCtx != Q_NULLPTR && m_codecCtx->framerate.den != 0)
        return (double) m_codecCtx->framerate.num / (double) m_codecCtx->framerate.den;
    else
        return 0;
}

int QFfmpegCodec::frameNumber() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->frame_number;
    else
        return -1;
}

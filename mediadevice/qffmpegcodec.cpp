#include "qffmpegcodec.h"

QFfmpegCodec::QFfmpegCodec(QObject *parent):
    QObject(parent)
{
    DebugInfo::add_object(this);
}

QFfmpegCodec::~QFfmpegCodec()
{
    DebugInfo::remove_object(this);
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
        if (m_codecCtx == Q_NULLPTR || avcodec_parameters_to_context(m_codecCtx, codecpar) != 0)
        {
            qCritical() << "unable to init codec";
            close();
            return false; // Error copying codec context
        }

        // open codec
        return open();
    }

    qCritical() << "invalid parameters" << codec << codecpar;
    return false;
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

        return true;
    }

    qCritical() << "unable to find codec" << codecName;
    return false;
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

        return true;
    }

    qCritical() << "unable to find codec" << id;
    return false;
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

            return true;
        }

        // codec already open
        qWarning() << "codec already open";
        return true;
    }

    close();
    return false;
}

AVMediaType QFfmpegCodec::type() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->codec_type;

    return AVMEDIA_TYPE_UNKNOWN;
}

AVSampleFormat QFfmpegCodec::sampleFormat() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->sample_fmt;

    return AV_SAMPLE_FMT_NONE;
}

AVPixelFormat QFfmpegCodec::pixelFormat() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->pix_fmt;

    return AV_PIX_FMT_NONE;
}

int QFfmpegCodec::channelCount() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->channels;

    return -1;
}

uint64_t QFfmpegCodec::channelLayout() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->channel_layout;

    return 0;
}

int QFfmpegCodec::samplerate() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->sample_rate;

    return -1;
}

qint64 QFfmpegCodec::bitrate() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->bit_rate;

    return -1;
}

QString QFfmpegCodec::format() const
{
    if (m_codecCtx != Q_NULLPTR && m_codecCtx->codec_descriptor != Q_NULLPTR)
        return m_codecCtx->codec_descriptor->name;

    return QString();
}

QString QFfmpegCodec::resolution() const
{
    if (m_codecCtx != Q_NULLPTR)
        return QString("%1x%2").arg(m_codecCtx->width).arg(m_codecCtx->height);

    return QString();
}

double QFfmpegCodec::frameRate() const
{
    if (m_codecCtx != Q_NULLPTR && m_codecCtx->framerate.den != 0)
        return static_cast<double>(m_codecCtx->framerate.num) / static_cast<double>(m_codecCtx->framerate.den);

    return 0.0;
}

int QFfmpegCodec::frameNumber() const
{
    if (m_codecCtx != Q_NULLPTR)
        return m_codecCtx->frame_number;

    return -1;
}

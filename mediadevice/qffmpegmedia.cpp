#include "qffmpegmedia.h"

bool QFfmpegMedia::m_initialised = false;

qint64 QFfmpegMedia::objectCounter = 0;

QFfmpegMedia::QFfmpegMedia()
{
    objectCounter++;

    init();
}

QFfmpegMedia::~QFfmpegMedia()
{
    objectCounter--;
}

void QFfmpegMedia::init()
{

#ifdef QT_DEBUG
    av_log_set_level(AV_LOG_DEBUG);
#else
    av_log_set_level(AV_LOG_ERROR);
#endif

    if (!m_initialised)
    {
        av_register_all();
        m_initialised = true;
    }
}

QString QFfmpegMedia::filename() const
{
    QString res;

    if (context() != Q_NULLPTR)
        res = QString(context()->filename);

    return res;
}

bool QFfmpegMedia::isValid() const
{
    return m_initialised && m_error.isEmpty();
}

void QFfmpegMedia::setParent(void *parent)
{
    m_parent = parent;
}

void *QFfmpegMedia::parent() const
{
    return m_parent;
}

void QFfmpegMedia::setError(const QString &error)
{
    m_error = error;
}

QString QFfmpegMedia::getVersion()
{
    return QString("libavformat %1.%2, libavcodec %3.%4").arg(LIBAVFORMAT_VERSION_MAJOR).arg(LIBAVFORMAT_VERSION_MINOR).arg(LIBAVCODEC_VERSION_MAJOR).arg(LIBAVCODEC_VERSION_MINOR);
}

bool QFfmpegMedia::isOpen()
{
    if (context() != Q_NULLPTR)
        return context()->pb != Q_NULLPTR;
    else
        return false;
}

QStringList QFfmpegMedia::getMetaDataStream(const AVMediaType &type, const QString &param) const
{
    QStringList res;

    if (context() != Q_NULLPTR)
    {
        for (unsigned int i=0; i<context()->nb_streams; ++i)
        {
            AVStream *stream = context()->streams[i];
            if (stream != Q_NULLPTR && stream->codecpar != Q_NULLPTR && stream->codecpar->codec_type == type)
            {
                bool tagFound = false;
                AVDictionaryEntry *entry = Q_NULLPTR;
                while ((entry = av_dict_get(stream->metadata, "", entry, AV_DICT_IGNORE_SUFFIX)))
                {
                    if (entry->key == param)
                    {
                        res << entry->value;
                        tagFound = true;
                        break;
                    }
                }

                if (!tagFound)
                    res << "";
            }
        }
    }

    return res;
}

QStringList QFfmpegMedia::getAudioLanguages() const
{
   return getMetaDataStream(AVMEDIA_TYPE_AUDIO, "language");
}

QStringList QFfmpegMedia::getVideoLanguages() const
{
    return getMetaDataStream(AVMEDIA_TYPE_VIDEO, "language");
}

QStringList QFfmpegMedia::getSubtitleLanguages() const
{
    return getMetaDataStream(AVMEDIA_TYPE_SUBTITLE, "language");
}

QString QFfmpegMedia::metaData(const QString &tagName) const
{
    if (context() != Q_NULLPTR)
    {
        AVDictionaryEntry *entry = Q_NULLPTR;
        while ((entry = av_dict_get(context()->metadata, "", entry, AV_DICT_IGNORE_SUFFIX)))
        {
            if (entry != Q_NULLPTR && entry->key == tagName)
                return  entry->value;
        }
    }

    return QString();
}

QHash<QString, double> QFfmpegMedia::getVolumeInfo(const int timeout)
{
    QHash<QString, double> res;
    return res;
}

QString QFfmpegMedia::getAudioFormat() const
{
    if (audioStream())
        return audioStream()->format();
    else
        return QString();
}

int QFfmpegMedia::getAudioChannelCount() const
{
    if (audioStream())
        return audioStream()->channelCount();
    else
        return -1;
}

int QFfmpegMedia::getAudioSamplerate() const
{
    if (audioStream())
        return audioStream()->samplerate();
    else
        return -1;
}

qint64 QFfmpegMedia::getAudioBitrate() const
{
    if (audioStream())
        return audioStream()->bitrate();
    else
        return -1;
}

QString QFfmpegMedia::getVideoResolution() const
{
    if (videoStream())
        return videoStream()->resolution();
    else
        return QString();
}

double QFfmpegMedia::getVideoFrameRate() const
{
    if (videoStream())
        return videoStream()->frameRate();
    else
        return 0.0;
}

#include "qffmpegprocess.h"

QString QFfmpegProcess::EXE_DIRPATH = "";

void QFfmpegProcess::setDirPath(const QString &folder)
{
    EXE_DIRPATH = folder;
}

QFfmpegProcess::QFfmpegProcess(QObject *parent):
    QObject(parent),
    programFfmpegProbe(Q_NULLPTR),
    programFfmpegPicture(Q_NULLPTR),
    filename(),
    xmlResProbe(),
    audioStream(),
    videoStream(),
    picture(Q_NULLPTR)
{
    ANALYZER

    if (EXE_DIRPATH.isEmpty())
    {
        qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries.";
    }
    else
    {
        QDir folder(EXE_DIRPATH);
        if (!folder.exists())
        {
            qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries:" << folder;
        }
    }

    ANALYZER_RETURN
}

QFfmpegProcess::QFfmpegProcess(const QString &filename, QObject *parent):
    QObject(parent),
    programFfmpegProbe(Q_NULLPTR),
    programFfmpegPicture(Q_NULLPTR),
    filename(),
    xmlResProbe(),
    audioStream(),
    videoStream(),
    picture(Q_NULLPTR)
{
    ANALYZER

    if (EXE_DIRPATH.isEmpty())
    {
        qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries.";
    }
    else
    {
        QDir folder(EXE_DIRPATH);
        if (!folder.exists())
        {
            qCritical() << "QFFmpeg, invalid installation path where are located FFMPEG binaries:" << folder;
        }
        else
        {
            setFilename(filename, true);
        }
    }

    ANALYZER_RETURN
}

QFfmpegProcess::~QFfmpegProcess()
{
    if (programFfmpegProbe)
    {
        programFfmpegProbe->disconnect(this);
        programFfmpegProbe->kill();
        programFfmpegProbe->waitForFinished();
    }

    if (programFfmpegPicture)
    {
        programFfmpegPicture->disconnect(this);
        programFfmpegPicture->kill();
        programFfmpegPicture->waitForFinished();
    }
}

bool QFfmpegProcess::isValid() const
{
    if (EXE_DIRPATH.isEmpty())
    {
        return false;
    }
    else
    {
        QDir folder(EXE_DIRPATH);
        return folder.exists();
    }
}

void QFfmpegProcess::probeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ANALYZER

    Q_UNUSED(exitCode)

    xmlResProbe.clear();
    audioStream.clear();
    videoStream.clear();

    QProcess *process = qobject_cast<QProcess*>(sender());

    if (process && exitStatus == QProcess::NormalExit)
    {
        xmlResProbe.setContent(process->readAll());
//        qDebug() << xmlResProbe.toString();

        QDomNodeList streamList = xmlResProbe.elementsByTagName("stream");
        for (int index=0;index<streamList.count();++index)
        {
            QDomNode stream = streamList.at(index);
            if (stream.attributes().namedItem("codec_type").nodeValue() == "audio")
                audioStream = stream;
            else if (stream.attributes().namedItem("codec_type").nodeValue() == "video")
                videoStream = stream;
        }
    }
    else
    {
        if (process)
        {
            qCritical() << "probe crashed" << exitCode << exitStatus << filename;
            qCritical() << process->program() << process->arguments();
            qCritical() << process->readAll();
        }
        else
        {
            qCritical() << "invalid process" << process;
        }
    }

    if (process)
        process->deleteLater();

    emit mediaReady();

    ANALYZER_RETURN
}

QString QFfmpegProcess::metaData(const QString &tagName) const
{
    ANALYZER

    if (waitProbeFinished())
    {
        QDomNodeList tagList = xmlResProbe.elementsByTagName("format").at(0).childNodes();

        for (int index=0;index<tagList.count();++index)
        {
            QDomNode tag = tagList.at(index);
            if (tag.nodeName() == "tag" && tag.attributes().namedItem("key").nodeValue() == tagName)
            {
                ANALYZER_RETURN
                        return tag.attributes().namedItem("value").nodeValue();
            }
        }
    }

    ANALYZER_RETURN
    return QString();
}

QString QFfmpegProcess::getFormat() const
{
    if (waitProbeFinished())
    {
        QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
        return format.attributes().namedItem("format_name").nodeValue();
    }
    else
    {
        return QString();
    }
}

qint64 QFfmpegProcess::size() const
{
    if (waitProbeFinished())
    {
        QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
        return format.attributes().namedItem("size").nodeValue().toLongLong();
    }
    else
    {
        return -1;
    }
}

int QFfmpegProcess::getBitrate() const
{
    if (waitProbeFinished())
    {
        QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
        return format.attributes().namedItem("bit_rate").nodeValue().toInt();
    }
    else
    {
        return -1;
    }
}

QString QFfmpegProcess::getAudioFormat() const
{
    ANALYZER

    QString res;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("codec_name").nodeValue();

    ANALYZER_RETURN
    return res;
}

int QFfmpegProcess::getAudioBitrate() const
{
    ANALYZER

    int res = -1;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("bit_rate").nodeValue().toInt();

    ANALYZER_RETURN
    return res;
}

int QFfmpegProcess::getAudioChannelCount() const
{
    ANALYZER

    int res = -1;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("channels").nodeValue().toInt();

    ANALYZER_RETURN
    return res;
}

int QFfmpegProcess::getAudioSamplerate() const
{
    ANALYZER

    int res = -1;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("sample_rate").nodeValue().toInt();

    ANALYZER_RETURN
    return res;
}

QString QFfmpegProcess::getVideoResolution() const
{
    if (waitProbeFinished())
    {
        QString width = videoStream.attributes().namedItem("width").nodeValue();
        QString height = videoStream.attributes().namedItem("height").nodeValue();
        if (!width.isEmpty() && !height.isEmpty())
            return QString("%1x%2").arg(width).arg(height);
    }
    return QString();
}


double QFfmpegProcess::getVideoFrameRate() const
{
    if (waitProbeFinished())
    {
        QRegExp pattern("(\\d+)/(\\d+)");
        QString strFrameRate = videoStream.attributes().namedItem("avg_frame_rate").nodeValue();
        if (pattern.indexIn(strFrameRate) != -1 && pattern.cap(2).toDouble() != 0.0)
            return pattern.cap(1).toDouble()/pattern.cap(2).toDouble();
        else
            return 0.0;
    }
    else
    {
        return 0.0;
    }
}

QStringList QFfmpegProcess::getStreamsTag(const QString &codec_type, const QString &tagName) const
{
    QStringList res;

    if (waitProbeFinished())
    {
        QDomNodeList streamList = xmlResProbe.elementsByTagName("stream");
        for (int index=0;index<streamList.count();++index)
        {
            QDomNode stream = streamList.at(index);

            if (stream.attributes().namedItem("codec_type").nodeValue() == codec_type)
            {
                QString value;

                QDomNodeList tagList = stream.childNodes();
                for (int num_child=0;num_child<tagList.count();++num_child)
                {
                    QDomNode tag = tagList.at(num_child);
                    if (tag.nodeName() == "tag" && tag.attributes().namedItem("key").nodeValue().toLower() == tagName)
                        value = tag.attributes().namedItem("value").nodeValue().toLower();
                }

                res << value;
            }
        }
    }

    return res;
}

qint64 QFfmpegProcess::getDuration() const
{
    ANALYZER

    qint64 res = -1;

    if (waitProbeFinished())
        res = xmlResProbe.elementsByTagName("format").at(0).attributes().namedItem("duration").nodeValue().toDouble()*1000.0;

    ANALYZER_RETURN
    return res;
}

void QFfmpegProcess::parsePicture()
{
    ANALYZER

    picture->clear();

    if (isValid() && !filename.isEmpty())
    {
        programFfmpegPicture = new QProcess(this);
        connect(programFfmpegPicture, SIGNAL(destroyed(QObject*)), this, SLOT(pictureDestroyed()));
        connect(programFfmpegPicture, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(pictureFinished(int,QProcess::ExitStatus)));

        programFfmpegPicture->setProgram(QDir(EXE_DIRPATH).absoluteFilePath("ffmpeg"));

        QStringList arguments = QStringList() << "-i" << filename << "-f" << "mjpeg" << "-vframes" << "1" << "-s" << "300x300" << "-loglevel" << "error" << "pipe:";
        programFfmpegPicture->setArguments(arguments);

        programFfmpegPicture->start();
    }

    ANALYZER_RETURN
}

void QFfmpegProcess::pictureFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ANALYZER

    Q_UNUSED(exitCode)

    QProcess *process = qobject_cast<QProcess*>(sender());

    if (process && exitStatus == QProcess::NormalExit)
    {
        if (exitCode == 0)
        {
            picture->append(process->readAllStandardOutput());

            if (picture->isEmpty())
                qWarning() << "NO PICTURE" << exitCode << filename;
        }
    }
    else
    {
        if (process)
            qCritical() << "picture parsing crashed" << filename;
        else
            qCritical() << "invalid process" << process;
    }

    if (process)
        process->deleteLater();

    ANALYZER_RETURN
}

QByteArray QFfmpegProcess::getPicture() const {
    ANALYZER

    if (waitPictureFinished())
    {
        if (!picture)
        {
            ANALYZER_RETURN
            return QByteArray();
        }

        if (!picture->isNull()) {
            if (picture->isEmpty())
            {
                ANALYZER_RETURN
                return QByteArray();
            }
            else
            {
                ANALYZER_RETURN
                return *picture;
            }
        }

        ANALYZER_RETURN
        return *picture;
    }

    ANALYZER_RETURN
    return QByteArray();
}

QHash<QString, double> QFfmpegProcess::getVolumeInfo(const int timeout)
{
    ANALYZER

    QHash<QString, double> result;

    if (isValid() && !filename.isEmpty())
    {
        QProcess process;
        process.setProgram(QDir(EXE_DIRPATH).absoluteFilePath("ffmpeg"));
        QStringList arguments = QStringList() << "-hide_banner" << "-nostats";
        arguments << "-i" << filename;
        arguments << "-map" << "0:a" << "-af" << "volumedetect";
        arguments << "-to" << "1800";  // parse only 30 minutes
        arguments << "-f" << "null" << "/dev/null";
        process.setArguments(arguments);
        process.start();

        if (process.waitForFinished(timeout))
        {
            if (process.exitCode() != 0)
            {
                QString error(process.readAllStandardError().trimmed());
                qCritical() << "ERROR" << process.exitCode() << filename << error;
            }
            else
            {
                QRegularExpression pattern("\\[Parsed_volumedetect_0.*\\]\\s*(\\w+):\\s*([\\+\\-\\.\\w]+)");

                QString data = process.readAllStandardError();

                QRegularExpressionMatchIterator iterator = pattern.globalMatch(data);
                while (iterator.hasNext())
                {
                    QRegularExpressionMatch match = iterator.next();
                    result[match.captured(1)] = match.captured(2).toDouble();
                }
            }
        }
        else
        {
            qWarning() << "NO VOLUME INFO" << filename;
            process.close();
        }
    }

    ANALYZER_RETURN
    return result;
}

bool QFfmpegProcess::setFilename(const QString &filename, const bool &readPicture)
{
    ANALYZER

    xmlResProbe.clear();
    audioStream.clear();
    videoStream.clear();
    if (picture)
        picture->clear();

    if (isValid() && !filename.isEmpty())
    {
        programFfmpegProbe = new QProcess(this);
        connect(programFfmpegProbe, SIGNAL(destroyed(QObject*)), this, SLOT(probeDestroyed()));
        connect(programFfmpegProbe, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(probeFinished(int,QProcess::ExitStatus)));

        programFfmpegProbe->setProgram(QDir(EXE_DIRPATH).absoluteFilePath("ffprobe"));

        QStringList argFfmpeg;
        argFfmpeg << "-i" << filename;
        argFfmpeg << "-show_format";
        argFfmpeg << "-show_entries" << "stream";
        argFfmpeg << "-of" << "xml";
        programFfmpegProbe->setArguments(argFfmpeg);

        programFfmpegProbe->start();

        this->filename = filename;

        if (readPicture)
        {
            if (!picture)
                picture = new QByteArray();
            parsePicture();
        }
        else
        {
            if (picture)
            {
                delete picture;
                picture = Q_NULLPTR;
            }
        }
    }

    ANALYZER_RETURN
    return true;
}

bool QFfmpegProcess::waitProbeFinished() const
{
    ANALYZER
    if (programFfmpegProbe)
    {
        if (programFfmpegProbe->state() != QProcess::NotRunning)
        {
            bool res = programFfmpegProbe->waitForFinished();
            ANALYZER_RETURN
            return res;
        }
        else
        {
            ANALYZER_RETURN
            return true;
        }
    }
    else
    {
        ANALYZER_RETURN
        return true;
    }
}

void QFfmpegProcess::probeDestroyed()
{
    programFfmpegProbe = Q_NULLPTR;
}

bool QFfmpegProcess::waitPictureFinished() const
{
    if (programFfmpegPicture)
    {
        if (programFfmpegPicture->state() != QProcess::NotRunning)
            return programFfmpegPicture->waitForFinished();
        else
            return true;
    }
    else
    {
        return true;
    }
}

void QFfmpegProcess::pictureDestroyed()
{
    programFfmpegPicture = Q_NULLPTR;
}

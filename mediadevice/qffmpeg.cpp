#include "qffmpeg.h"

QString QFfmpeg::EXE_DIRPATH = "";

void QFfmpeg::setDirPath(const QString &folder)
{
    EXE_DIRPATH = folder;
}

QString QFfmpeg::getVersion()
{
    QDir folder(EXE_DIRPATH);
    if (folder.exists())
    {
        QProcess process;
        process.setProgram(QDir(EXE_DIRPATH).absoluteFilePath("ffmpeg"));

        QStringList args;
        args << "-version";
        process.setArguments(args);

        process.start();
        if (process.waitForFinished() && process.exitCode() == 0)
        {
            QByteArray data = process.readLine();
            return data;
        }
    }

    return QString();
}

QFfmpeg::QFfmpeg(QObject *parent):
    QObject(parent),
    filename()
{
    ANALYZER;

    DebugInfo::add_object(this);

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
}

QFfmpeg::QFfmpeg(const QString &filename, QObject *parent):
    QObject(parent),
    filename()
{
    ANALYZER;

    DebugInfo::add_object(this);

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
}

QFfmpeg::~QFfmpeg()
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

bool QFfmpeg::isValid() const
{
    if (EXE_DIRPATH.isEmpty())
        return false;

    QDir folder(EXE_DIRPATH);
    return folder.exists();
}

void QFfmpeg::probeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ANALYZER;

    Q_UNUSED(exitCode)

    xmlResProbe.clear();
    audioStream.clear();
    videoStream.clear();

    auto process = qobject_cast<QProcess*>(sender());

    if (process && exitStatus == QProcess::NormalExit)
    {
        xmlResProbe.setContent(process->readAll());

#if !defined(QT_NO_DEBUG_OUTPUT)
        qDebug() << xmlResProbe.toString();
#endif

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
}

QString QFfmpeg::metaData(const QString &tagName) const
{
    ANALYZER;

    if (waitProbeFinished())
    {
        QDomNodeList tagList = xmlResProbe.elementsByTagName("format").at(0).childNodes();

        for (int index=0;index<tagList.count();++index)
        {
            QDomNode tag = tagList.at(index);
            if (tag.nodeName() == "tag" && tag.attributes().namedItem("key").nodeValue() == tagName)
            {
                return tag.attributes().namedItem("value").nodeValue();
            }
        }
    }

    return QString();
}

QString QFfmpeg::getFormat() const
{
    if (waitProbeFinished())
    {
        QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
        return format.attributes().namedItem("format_name").nodeValue();
    }

    return QString();
}

qint64 QFfmpeg::size() const
{
    if (waitProbeFinished())
    {
        QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
        return format.attributes().namedItem("size").nodeValue().toLongLong();
    }

    return -1;
}

int QFfmpeg::getBitrate() const
{
    if (waitProbeFinished())
    {
        QDomNode format = xmlResProbe.elementsByTagName("format").at(0);
        return format.attributes().namedItem("bit_rate").nodeValue().toInt();
    }

    return -1;
}

QString QFfmpeg::getAudioFormat() const
{
    ANALYZER;

    QString res;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("codec_name").nodeValue();

    return res;
}

int QFfmpeg::getAudioBitrate() const
{
    ANALYZER;

    int res = -1;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("bit_rate").nodeValue().toInt();

    return res;
}

int QFfmpeg::getAudioChannelCount() const
{
    ANALYZER;

    int res = -1;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("channels").nodeValue().toInt();

    return res;
}

int QFfmpeg::getAudioSamplerate() const
{
    ANALYZER;

    int res = -1;

    if (waitProbeFinished())
        res = audioStream.attributes().namedItem("sample_rate").nodeValue().toInt();

    return res;
}

QString QFfmpeg::getVideoResolution() const
{
    if (waitProbeFinished())
    {
        QString width = videoStream.attributes().namedItem("width").nodeValue();
        QString height = videoStream.attributes().namedItem("height").nodeValue();
        if (!width.isEmpty() && !height.isEmpty())
            return QString("%1x%2").arg(width, height);
    }
    return QString();
}


double QFfmpeg::getVideoFrameRate() const
{
    if (waitProbeFinished())
    {
        QRegExp pattern("(\\d+)/(\\d+)");
        QString strFrameRate = videoStream.attributes().namedItem("avg_frame_rate").nodeValue();
        if (pattern.indexIn(strFrameRate) != -1 && pattern.cap(2).toDouble() != 0.0)
            return pattern.cap(1).toDouble()/pattern.cap(2).toDouble();
    }

    return 0.0;
}

QStringList QFfmpeg::getStreamsTag(const QString &codec_type, const QString &tagName) const
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

qint64 QFfmpeg::getDuration() const
{
    ANALYZER;

    qint64 res = -1;

    if (waitProbeFinished())
        res = static_cast<qint64>(xmlResProbe.elementsByTagName("format").at(0).attributes().namedItem("duration").nodeValue().toDouble()*1000.0);

    return res;
}

void QFfmpeg::parsePicture()
{
    ANALYZER;

    picture->clear();

    if (isValid() && !filename.isEmpty())
    {
        programFfmpegPicture = new QProcess(this);
        DebugInfo::add_object(programFfmpegPicture);
        connect(programFfmpegPicture, SIGNAL(destroyed(QObject*)), this, SLOT(pictureDestroyed()));
        connect(programFfmpegPicture, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(pictureFinished(int,QProcess::ExitStatus)));

        programFfmpegPicture->setProgram(QDir(EXE_DIRPATH).absoluteFilePath("ffmpeg"));

        QStringList arguments = QStringList() << "-i" << filename << "-f" << "mjpeg" << "-vframes" << "1" << "-s" << "300x300" << "-loglevel" << "error" << "pipe:";
        programFfmpegPicture->setArguments(arguments);

        programFfmpegPicture->start();
    }

}

void QFfmpeg::pictureFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ANALYZER;

    Q_UNUSED(exitCode)

    auto process = qobject_cast<QProcess*>(sender());

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

}

QByteArray QFfmpeg::getPicture() const {
    ANALYZER;

    if (waitPictureFinished())
    {
        if (!picture)
        {
            return QByteArray();
        }

        if (!picture->isNull()) {
            if (picture->isEmpty())
            {
                return QByteArray();
            }

            return *picture;
        }

        return *picture;
    }

    return QByteArray();
}

QHash<QString, double> QFfmpeg::getVolumeInfo(const int& timeout)
{
    ANALYZER;

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
                QRegularExpression pattern(R"(\[Parsed_volumedetect_0.*\]\s*(\w+):\s*([\+\-\.\w]+))");

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

    return result;
}

bool QFfmpeg::setFilename(const QString &filename, const bool &readPicture)
{
    ANALYZER;

    xmlResProbe.clear();
    audioStream.clear();
    videoStream.clear();
    if (picture)
        picture->clear();

    if (isValid() && !filename.isEmpty())
    {
        programFfmpegProbe = new QProcess(this);
        DebugInfo::add_object(programFfmpegProbe);
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

    return true;
}

bool QFfmpeg::waitProbeFinished() const
{
    ANALYZER;
    if (programFfmpegProbe)
    {
        if (programFfmpegProbe->state() != QProcess::NotRunning)
        {
            bool res = programFfmpegProbe->waitForFinished();
            return res;
        }
    }

    return true;
}

void QFfmpeg::probeDestroyed()
{
    programFfmpegProbe = Q_NULLPTR;
}

bool QFfmpeg::waitPictureFinished() const
{
    if (programFfmpegPicture)
    {
        if (programFfmpegPicture->state() != QProcess::NotRunning)
            return programFfmpegPicture->waitForFinished();
    }

    return true;
}

void QFfmpeg::pictureDestroyed()
{
    programFfmpegPicture = Q_NULLPTR;
}

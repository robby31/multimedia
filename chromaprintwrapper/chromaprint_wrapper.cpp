#include "chromaprint_wrapper.h"

ChromaprintWrapper::ChromaprintWrapper():
    algo(CHROMAPRINT_ALGORITHM_DEFAULT),
    chromaprint_ctx(chromaprint_new(algo)),
    programFfmpeg()
{
}

ChromaprintWrapper::~ChromaprintWrapper()
{
    chromaprint_free(chromaprint_ctx);
}

QString ChromaprintWrapper::get_version() const
{
    return chromaprint_get_version();
}

int ChromaprintWrapper::decode_audio_file(ChromaprintContext *chromaprint_ctx, const QString &file_name, const int &max_length, int *p_duration)
{
    programFfmpeg.setProgram("/Users/doudou/workspaceQT/MyLibrary/5.13.0/bin");

    QStringList argFfmpeg;
    argFfmpeg << "-i" << file_name;
    argFfmpeg << "-show_format";
    argFfmpeg << "-show_entries" << "stream";
    argFfmpeg << "-of" << "xml";
    programFfmpeg.setArguments(argFfmpeg);
    programFfmpeg.start();
    if (!programFfmpeg.waitForFinished())
    {
        qWarning() << "probe failed" << file_name;
        return 0;
    }
    else
    {
        int channels = -1;
        int sample_rate = -1;
        int duration = -1;

        QDomDocument xmlResProbe;
        xmlResProbe.setContent(programFfmpeg.readAll());

        QDomNode audioStream;
        QDomNodeList streamList = xmlResProbe.elementsByTagName("stream");
        for (int index=0;index<streamList.count();++index)
        {
            QDomNode stream = streamList.at(index);
            if (stream.attributes().namedItem("codec_type").nodeValue() == "audio")
                audioStream = stream;
        }

        channels = audioStream.attributes().namedItem("channels").nodeValue().toInt();
        sample_rate = audioStream.attributes().namedItem("sample_rate").nodeValue().toInt();
        duration = xmlResProbe.elementsByTagName("format").at(0).attributes().namedItem("duration").nodeValue().toDouble();

        if (channels>0 && sample_rate>0 && duration>0)
        {
            *p_duration = duration;

            chromaprint_start(chromaprint_ctx, sample_rate, channels);

            // decode audio
            programFfmpeg.setProgram("/Users/doudou/workspaceQT/MyLibrary/5.13.0/bin");

            QStringList argFfmpeg;
            argFfmpeg << "-i" << file_name;
            argFfmpeg << "-map" << "0:a";
            argFfmpeg << "-f" << "s16le";
            argFfmpeg << "-to" << QString("%1").arg(max_length);
            argFfmpeg << "pipe:";
            programFfmpeg.setArguments(argFfmpeg);

            programFfmpeg.start();
            if (!programFfmpeg.waitForFinished())
            {
                qWarning() << "audio decoding failed" << file_name;
                return 0;
            } else {
                if (programFfmpeg.exitStatus() == QProcess::NormalExit)
                {
                    QByteArray data = programFfmpeg.readAllStandardOutput();

                    int nb_samples = data.size()*8/16/channels;

                    if (!chromaprint_feed(chromaprint_ctx, (int16_t*)data.data(), nb_samples))
                    {
                        qWarning() << "error in reading audio data" << file_name;
                        return 0;
                    }
                    else {
                        if (!chromaprint_finish(chromaprint_ctx))
                        {
                            qWarning() << "ERROR: fingerprint calculation failed" << file_name;
                            return 0;
                        }
                        else
                        {
                            // succeed
                            return 1;
                        }
                    }
                }
                else
                {
                    qWarning() << "audio decoding failed" << file_name;
                    return 0;
                }
            }
        }
        else
        {
            qWarning() << "duration, channels or sample_rate is incorrect" << duration << channels << sample_rate;
            return 0;
        }
    }

    // default code return (FAIL)
    return 0;
}

QString ChromaprintWrapper::fingerPrint(const QString &file_name, int *duration, const int &max_length)
{
    QString fingerprint;

    if (!decode_audio_file(chromaprint_ctx, file_name, max_length, duration)) {
        qWarning() << "ERROR: unable to calculate fingerprint for file" << file_name;
    } else {

        char *tmp_fingerprint;
        if (!chromaprint_get_fingerprint(chromaprint_ctx, &tmp_fingerprint)) {
            qWarning() << "ERROR: unable to calculate fingerprint for file" << file_name;
        } else {
            fingerprint = QString::fromLatin1(tmp_fingerprint);
            chromaprint_dealloc(tmp_fingerprint);
        }
    }

    return fingerprint;
}

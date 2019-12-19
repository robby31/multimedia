#include "ffmpegtranscoding.h"

QString FfmpegTranscoding::EXE_DIRPATH = "";

void FfmpegTranscoding::setDirPath(const QString &folder)
{
    EXE_DIRPATH = folder;
}

FfmpegTranscoding::FfmpegTranscoding(QObject *parent) :
    TranscodeProcess(parent)
{
    QDir folder(EXE_DIRPATH);
    setProgram(folder.absoluteFilePath("ffmpeg"));
}

void FfmpegTranscoding::updateArguments()
{
    QString ssOption;
    if (timeSeekStart() > 0)
    {
        ssOption = QString("%1").arg(timeSeekStart());
    }
    else if (startByte() > 0)
    {
        if (lengthInSeconds() > 0 && fullSize() > 0)
        {
            if (startByte() <= fullSize())
            {
                double start_position = double(startByte())/double(fullSize())*double(lengthInSeconds());
                ssOption = QString("%1").arg(qCeil(start_position));
            }
            else
            {
                qWarning() << "startByte is greater than size" << startByte() << fullSize();
            }
        }
        else
        {
            qWarning() << "unable to set startByte" << startByte() << ", length is not set" << lengthInSeconds() << "or size is not valid" << fullSize();
        }
    }

    QStringList arguments;
//    arguments << "-loglevel" << "error";
    arguments << "-hide_banner";
    arguments << "-nostats";

//    arguments << "-probesize" << "5000";
//    arguments << "-analyzeduration" << "0";

    QString subtitle_url;
    if (!url().isEmpty())
    {
        for (const QUrl &url : url())
        {
            if (!ssOption.isEmpty())
                arguments << "-ss" << ssOption;

            arguments << "-i" << url.url();
            if (subtitle_url.isEmpty())
                subtitle_url = url.url();
        }
    }

    if (format() == MP3)
    {
        arguments << "-vn";  // no video only audio
        arguments << "-map_metadata" << "-1";

        arguments << "-f" << "mp3" << "-codec:a" << "libmp3lame";

        if (bitrate() > 0)
            arguments << "-b:a" << QString("%1").arg(bitrate());

    }
    else if (format() == AAC)
    {
        arguments << "-vn";  // no video only audio
        arguments << "-map_metadata" << "-1";

        arguments << "-f" << "ipod" << "-codec:a" << "libfdk_aac" << "-movflags" << "+faststart";

        if (bitrate() > 0)
            arguments << "-b:a" << QString("%1").arg(bitrate());
    }
    else if (format() == ALAC)
    {
        arguments << "-vn";  // no video only audio
        arguments << "-map_metadata" << "-1";

        arguments << "-f" << "ipod" << "-codec:a" << "alac" << "-sample_fmt" << "s16p" << "-movflags" << "frag_keyframe+empty_moov+faststart";

        if (bitrate() > 0)
            arguments << "-b:a" << QString("%1").arg(bitrate());
    }
    else if (format() == LPCM_S16BE)
    {
        arguments << "-vn";  // no video only audio
        arguments << "-map_metadata" << "-1";

        arguments << "-f" << "s16be";

        if (bitrate() > 0)
            arguments << "-b:a " << QString("%1").arg(bitrate());
    }
    else if (format() == LPCM_S16LE)
    {
        arguments << "-vn";  // no video only audio
        arguments << "-map_metadata" << "-1";

        arguments << "-f" << "s16le";

        if (bitrate() > 0)
            arguments << "-b:a " << QString("%1").arg(bitrate());
    }
    else if (format() == WAV)
    {
        arguments << "-vn";  // no video only audio
        arguments << "-map_metadata" << "-1";

        arguments << "-f" << "wav";

        if (bitrate() > 0)
            arguments << "-b:a" << QString("%1").arg(bitrate());
    }
    else if (format() == MPEG2_AC3 || format() == H264_AAC || format() == H264_AC3)
    {
        if (!audioLanguages().contains("fre") && !audioLanguages().contains("fra"))
        {
            // select subtitle
            int subtitleStreamIndex = -1;
            if (subtitleLanguages().contains("fre"))
                subtitleStreamIndex = subtitleLanguages().indexOf("fre");
            else if (subtitleLanguages().contains("fra"))
                subtitleStreamIndex = subtitleLanguages().indexOf("fra");
            else if (subtitleLanguages().contains("eng"))
                subtitleStreamIndex = subtitleLanguages().indexOf("eng");

            if (subtitleStreamIndex >= 0)
            {
                if (ssOption.isEmpty())
                    arguments << "-vf" << QString("subtitles=%1:si=%2").arg(subtitle_url).arg(subtitleStreamIndex);
                else
                    arguments << "-vf" << QString("setpts=PTS+%3/TB,subtitles=%1:si=%2,setpts=PTS-STARTPTS").arg(subtitle_url).arg(subtitleStreamIndex).arg(ssOption);
            }


            if (audioLanguages().contains("eng"))
            {
                // select english language for audio
                arguments << "-map" << QString("0:a:%1").arg(audioLanguages().indexOf("eng"));
                arguments << "-map" << "0:v:0";
            }
        }
        else
        {
            // select french language for audio
            if (audioLanguages().contains("fre"))
                arguments << "-map" << QString("0:a:%1").arg(audioLanguages().indexOf("fre"));
            else if (audioLanguages().contains("fra"))
                arguments << "-map" << QString("0:a:%1").arg(audioLanguages().indexOf("fra"));
            arguments << "-map" << "0:v:0";
        }

        // set container format to MPEGTS
        arguments << "-f" << "mpegts";

//        arguments << "-s" << "1920x1080";
//        arguments << "-s" << "1280x720";
//        arguments << "-s" << "736x414";
//        arguments << "-s" << "640x360";

        // set frame rate
        double framerate = 25.0;
        if (frameRate() == "23.976" || frameRate() == "24000/1001")
        {
//            arguments << "-r" << "24000/1001";
            framerate = 23.976;
        }
        else if (frameRate() == "29.970" || frameRate() == "30000/1001")
        {
//            arguments << "-r" << "30000/1001";
            framerate = 29.97;
        }
        else if (frameRate() == "30.000" || frameRate() == "30/1")
        {
//            arguments << "-r" << "30";
            framerate = 30.0;
        }
        else if (frameRate() == "25.000" || frameRate() == "25/1")
        {
//            arguments << "-r" << "25";
            framerate = 25.0;
        }
        else
        {
            // default framerate output
//            arguments << "-r" << "25";
            framerate = 25.0;
            qInfo() << QString("Use default framerate (%1)").arg(frameRate());
        }

        // set audio options
        int audio_bitrate = 160000;  // default value for audio bitrate
        if (audioChannelCount() > 0)
            audio_bitrate = audioChannelCount()*96000;   // if audio channel count is valid, define bitrate = channel * 96kb/s

        if (format() == MPEG2_AC3 || format() == H264_AC3)
        {
            arguments << "-c:a" << "ac3";
        }
        else if (format() == H264_AAC)
        {
            arguments << "-c:a" << "libfdk_aac" << "-movflags" << "+faststart";
        }

        arguments << "-b:a" << QString("%1").arg(audio_bitrate);

//        if (audioSampleRate() > 0)
//            arguments << "-ar" << QString("%1").arg(audioSampleRate());

//        if (audioChannelCount() > 0)
//            arguments << "-ac" << QString("%1").arg(audioChannelCount());

        // set video options
        if (format() == MPEG2_AC3)
        {
            arguments << "-c:v" << "mpeg2video";
        }
        else if (format() == H264_AAC || format() == H264_AC3)
        {
            arguments << "-c:v" << "libx264";

            arguments << "-profile:v" << "baseline" << "-level" << "4.0";
//            arguments << "-preset" << "ultrafast";
//            arguments << "-tune" << "zerolatency";
        }

        if (!variable_bitrate)
        {
            // constant bitrate
            qint64 video_bitrate = (bitrate()-audio_bitrate);

            arguments << "-b:v" << QString("%1").arg(video_bitrate);
            arguments << "-minrate" << QString("%1").arg(video_bitrate);
            arguments << "-maxrate" << QString("%1").arg(video_bitrate);

            if (format() == H264_AAC || format() == H264_AC3)
            {
                arguments << "-bufsize" << QString("%1").arg(double(video_bitrate)/framerate);
                arguments << "-nal-hrd" << "cbr";
            }
            else
            {
                arguments << "-bufsize" << "1835k";
            }
        }
        else
        {
            // default : variable bitrate
            if (format() == H264_AAC || format() == H264_AC3)
            {
                arguments << "-crf" << "18";   // Very High quality
//                arguments << "-crf" << "23";   // High quality
//                arguments << "-crf" << "28";   // Low quality

            }
            else
            {
                arguments << "-qscale:v" << "3";
            }
        }
    }
    else if (format() == COPY)
    {
        // set container format to MPEGTS
        if (containerFormat() == MP4)
            arguments << "-f" << "mp4";
        else if (containerFormat() == MPEGTS)
            arguments << "-f" << "mpegts";

        arguments << "-c:a" << "copy";
        arguments << "-c:v" << "copy";

    }
    else
    {
        qCritical() << QString("Invalid format: %1").arg(format());
    }

    if (timeSeekEnd() > 0)
    {
        arguments << "-to" << QString("%1").arg(lengthInSeconds());
    }
    else if (endByte() >= 0)
    {
        if (size() > 0)
        {
            if (endByte() >= 0)
                arguments << "-fs" << QString("%1").arg(size());
        }
        else
        {
            // invalid length
            arguments << "-fs 0";
            qWarning() << "invalid size" << size();
        }
    }

    // normalize audio
//    arguments << "-af" << "dynaudnorm";

    if (m_output.isEmpty())
        arguments << "pipe:";
    else
        arguments << m_output;

    setArguments(arguments);
}

void FfmpegTranscoding::setVariableBitrate(const bool &flag)
{
    variable_bitrate = flag;
}

void FfmpegTranscoding::setOutput(const QString &output)
{
    m_output = output;
}

#include <QString>
#include <QtTest>
#include <QImage>

#include "qffmpeg.h"
#include "qffmpeginputmedia.h"
#include "qffmpegoutputmedia.h"

class ffmpegTest : public QObject
{
    Q_OBJECT

public:
    ffmpegTest();

private Q_SLOTS:
    void cleanup();

    void testQFfmpeg_data();
    void testQFfmpeg();
    void testQFfmpegMedia_data();
    void testQFfmpegMedia();
    void testQFfmpegMediaEncodeFileWAV_data();
    void testQFfmpegMediaEncodeFileWAV();
    void testQFfmpegMediaEncodeFileMP3_data();
    void testQFfmpegMediaEncodeFileMP3();
    void testQFfmpegMediaEncodeFileMP3WithImage_data();
    void testQFfmpegMediaEncodeFileMP3WithImage();
    void testQFfmpegMediaEncodeFileAAC_data();
    void testQFfmpegMediaEncodeFileAAC();
    void testQFfmpegMediaEncodeFileLPCM_data();
    void testQFfmpegMediaEncodeFileLPCM();
    void testQFfmpegMediaEncodeFileALAC_data();
    void testQFfmpegMediaEncodeFileALAC();

    void testQFfmpegMediaEncodeBufferWAV_data();
    void testQFfmpegMediaEncodeBufferWAV();
    void testQFfmpegMediaEncodeBufferMP3_data();
    void testQFfmpegMediaEncodeBufferMP3();
    void testQFfmpegMediaEncodeBufferAAC_data();
    void testQFfmpegMediaEncodeBufferAAC();
    void testQFfmpegMediaEncodeBufferLPCM_data();
    void testQFfmpegMediaEncodeBufferLPCM();
    void testQFfmpegMediaEncodeBufferALAC_data();
    void testQFfmpegMediaEncodeBufferALAC();
};

void ffmpegTest::cleanup()
{
    QCOMPARE(QFfmpegMedia::objectCounter, 0);
    QCOMPARE(QFfmpegStream::objectCounter, 0);
    QCOMPARE(QFfmpegCodec::objectCounter, 0);
    QCOMPARE(QFfmpegBuffer::objectCounter, 0);
    QCOMPARE(QFfmpegFrame::objectCounter, 0);
}

ffmpegTest::ffmpegTest()
{
    QFfmpeg::setDirPath("/opt/local/bin");
}

void ffmpegTest::testQFfmpeg_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<QString>("format");
    QTest::addColumn<int>("duration");
    QTest::addColumn<QString>("audioFormat");
    QTest::addColumn<int>("AudioChannelCount");
    QTest::addColumn<int>("AudioSampleRate");
    QTest::addColumn<int>("AudioBitrate");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<QString>("videoResolution");
    QTest::addColumn<double>("videoFrameRate");
    QTest::addColumn<QStringList>("videoLanguages");
    QTest::addColumn<QStringList>("audioLanguages");
    QTest::addColumn<QStringList>("subtitleLanguages");
    QTest::addColumn<int>("pictureSize");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("album");
    QTest::addColumn<QString>("track");
    QTest::addColumn<QString>("disc");
    QTest::addColumn<QString>("genre");
    QTest::addColumn<QString>("TLEN");
    QTest::addColumn<QString>("date");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << "mp3" << 159373 << "mp3" << 2 << 44100 << 189255 << 192808 << "500x500" << 0.0 << QStringList("") << QStringList("") << QStringList() << 4555 << QString("Phébus") << QString("-M-") << QString("Mister Mystère") << QString("2/13") << QString("1/2") << QString("Rock  Français") << QString("159320") << QString("2009-09-07");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << "mov,mp4,m4a,3gp,3g2,mj2" << 188987 << "aac" << 2 << 44100 << 124436 << 135074 << "300x300" << 0.0 << QStringList("") << QStringList("und") << QStringList() << 20406 << QString("Monde virtuel") << QString("-M-") << QString("Je dis aime") << QString("1/15") << QString("1/1") << QString("Pop") << QString("") << QString("1999-10-25");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << "mov,mp4,m4a,3gp,3g2,mj2" << 275387 << "alac" << 2 << 48000 << 963753 << 964994 << "600x600" << 0.0 << QStringList("") << QStringList("und") << QStringList() << 8446 << "Give Life Back To Music" << "Daft Punk" << "Random Access Memories" << "1" << "" << "Euro House" << "" << "2013";
    QTest::newRow("Video_MKV") << QString("/Users/doudou/Movies/Films/District.9.2009.720p.BrRip.YIFY.mkv") << "matroska,webm" << 6735830 << "aac" << 2 << 48000 << 0 << 871427 << "1280x688" << 23.976023976023978 << QStringList("") << QStringList("") << QStringList("eng") << 762 << QString("District 9 - YIFY") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("");

    QStringList tmp;
    tmp << "spa" << "eng" << "fre";
    QTest::newRow("Video_MKV2") << QString("/Users/doudou/Movies/Films/Margin Call [BDrip m-1080p AC3 Esp-Eng-Fr+subs].mkv") << QString("matroska,webm") << 6419022 << "ac3" << 2 << 48000 << 112000 << 4293543 << QString("1920x1080") << 24.0 << QStringList("spa") << QStringList(tmp) << QStringList(tmp) << 3120 << QString("Margin Call m-1080p.guerrero36") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("");

}

void ffmpegTest::testQFfmpeg()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(QString, format);
    QFETCH(int, duration);
    QFETCH(QString, audioFormat);
    QFETCH(int, AudioChannelCount);
    QFETCH(int, AudioSampleRate);
    QFETCH(int, AudioBitrate);
    QFETCH(int, bitrate);
    QFETCH(QString, videoResolution);
    QFETCH(double, videoFrameRate);
    QFETCH(QStringList, videoLanguages);
    QFETCH(QStringList, audioLanguages);
    QFETCH(QStringList, subtitleLanguages);
    QFETCH(int, pictureSize);
    QFETCH(QString, title);
    QFETCH(QString, artist);
    QFETCH(QString, album);
    QFETCH(QString, track);
    QFETCH(QString, disc);
    QFETCH(QString, genre);
    QFETCH(QString, TLEN);
    QFETCH(QString, date);

    timer.start();
    QFfmpeg media(pathname);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDuration() << "ms";

    QCOMPARE(media.getFormat(), format);
    QCOMPARE(media.getDuration(), duration);
    QCOMPARE(media.getAudioChannelCount(), AudioChannelCount);
    QCOMPARE(media.getAudioSamplerate(), AudioSampleRate);
    QCOMPARE(media.getBitrate(), bitrate);
    QCOMPARE(media.getVideoResolution(), videoResolution);
    QCOMPARE(QVariant::fromValue(media.getVideoFrameRate()).toString(), QVariant::fromValue(videoFrameRate).toString());
    QCOMPARE(media.getAudioBitrate(), AudioBitrate);
    QCOMPARE(media.getVideoLanguages(), videoLanguages);
    QCOMPARE(media.getAudioLanguages(), audioLanguages);
    QCOMPARE(media.getSubtitleLanguages(), subtitleLanguages);
    QCOMPARE(media.getAudioFormat(), audioFormat);

    QImage image;
    QByteArray picture = media.getPicture();
    QCOMPARE(picture.size(), pictureSize);
    QCOMPARE(image.loadFromData(picture), true);

    QFile fd(QString("%1_QFfmpeg.jpeg").arg(format));
    fd.open(QIODevice::ReadWrite);
    fd.write(picture);
    fd.close();

    QCOMPARE(media.metaData("title"), title);
    QCOMPARE(media.metaData("artist"), artist);
    QCOMPARE(media.metaData("album"), album);
    QCOMPARE(media.metaData("track"), track);
    QCOMPARE(media.metaData("disc"), disc);
    QCOMPARE(media.metaData("genre"), genre);
    QCOMPARE(media.metaData("TLEN"), TLEN);
    QCOMPARE(media.metaData("date"), date);

    ANALYZER_DISPLAY_RESULTS;
}

void ffmpegTest::testQFfmpegMedia_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<QString>("format");
    QTest::addColumn<qint64>("duration");
    QTest::addColumn<qint64>("size");
    QTest::addColumn<QString>("audioFormat");
    QTest::addColumn<int>("AudioChannelCount");
    QTest::addColumn<int>("AudioSampleRate");
    QTest::addColumn<int>("AudioBitrate");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<QString>("videoResolution");
    QTest::addColumn<double>("videoFrameRate");
    QTest::addColumn<QStringList>("videoLanguages");
    QTest::addColumn<QStringList>("audioLanguages");
    QTest::addColumn<QStringList>("subtitleLanguages");
    QTest::addColumn<int>("pictureSize");
    QTest::addColumn<bool>("hasPicture");
    QTest::addColumn<QString>("title");
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("album");
    QTest::addColumn<QString>("track");
    QTest::addColumn<QString>("disc");
    QTest::addColumn<QString>("genre");
    QTest::addColumn<QString>("TLEN");
    QTest::addColumn<QString>("date");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << QString("mp3") << (qint64)159373061 << (qint64)3841064 << "mp3" << 2 << 44100 << 189255 << 192808 << QString("500x500") << 0.0 << QStringList("") << QStringList("") << QStringList() << 4537 << true << QString("Phébus") << QString("-M-") << QString("Mister Mystère") << QString("2/13") << QString("1/2") << QString("Rock  Français") << QString("159320") << QString("2009-09-07");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << QString("mov,mp4,m4a,3gp,3g2,mj2") << (qint64)188987211 << (qint64)3190914 << "aac" << 2 << 44100 << 124436 << 135074 << QString("300x300") << 0.0 << QStringList("") << QStringList("und") << QStringList() << 20388 << true << QString("Monde virtuel") << QString("-M-") << QString("Je dis aime") << QString("1/15") << QString("1/1") << QString("Pop") << QString("") << QString("1999-10-25");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << "mov,mp4,m4a,3gp,3g2,mj2" << (qint64)275387000 << (qint64)33218372 << "alac" << 2 << 48000 << 963753 << 964994 << "600x600" << 0.0 << QStringList("") << QStringList("und") << QStringList() << 8428 << true << "Give Life Back To Music" << "Daft Punk" << "Random Access Memories" << "1" << "" << "Euro House" << "" << "2013";
    QTest::newRow("Video_MKV") << QString("/Users/doudou/Movies/Films/District.9.2009.720p.BrRip.YIFY.mkv") << QString("matroska,webm") << (qint64)6735830000 << (qint64)733723671 << "aac" << 2 << 48000 << 0 << 871427 << QString("1280x688") << 23.976023976023978 << QStringList("") << QStringList("") << QStringList("eng") << 0 << false << QString("District 9 - YIFY") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("");

    QStringList tmp;
    tmp << "spa" << "eng" << "fre";
    QTest::newRow("Video_MKV2") << QString("/Users/doudou/Movies/Films/Margin Call [BDrip m-1080p AC3 Esp-Eng-Fr+subs].mkv") << QString("matroska,webm") << (qint64)6419022000 << (qint64)3445044042 << "ac3" << 6 << 48000 << 448000 << 4293543 << QString("1920x1080") << 24.0 << QStringList("spa") << QStringList(tmp) << QStringList(tmp) << 0 << false << QString("Margin Call m-1080p.guerrero36") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("");

}

void ffmpegTest::testQFfmpegMedia()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(QString, format);
    QFETCH(qint64, duration);
    QFETCH(qint64, size);
    QFETCH(QString, audioFormat);
    QFETCH(int, AudioChannelCount);
    QFETCH(int, AudioSampleRate);
    QFETCH(int, AudioBitrate);
    QFETCH(int, bitrate);
    QFETCH(QString, videoResolution);
    QFETCH(double, videoFrameRate);
    QFETCH(QStringList, videoLanguages);
    QFETCH(QStringList, audioLanguages);
    QFETCH(QStringList, subtitleLanguages);
    QFETCH(int, pictureSize);
    QFETCH(bool, hasPicture);
    QFETCH(QString, title);
    QFETCH(QString, artist);
    QFETCH(QString, album);
    QFETCH(QString, track);
    QFETCH(QString, disc);
    QFETCH(QString, genre);
    QFETCH(QString, TLEN);
    QFETCH(QString, date);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    QCOMPARE(media.filename(), pathname);
    QVERIFY(media.getVersion().startsWith("libavformat"));
    QCOMPARE(media.isValid(), true);
    QCOMPARE(media.getFormat(), format);
    QCOMPARE(media.getDurationInMicroSec(), duration);
    QCOMPARE(media.getBitrate(), bitrate);
    QCOMPARE(media.size(), size);

    QVERIFY(media.audioStream() != Q_NULLPTR);
    QCOMPARE(media.audioStream()->channelCount(), AudioChannelCount);
    QCOMPARE(media.audioStream()->samplerate(), AudioSampleRate);
    QCOMPARE(media.audioStream()->bitrate(), AudioBitrate);
    QCOMPARE(media.audioStream()->format(), audioFormat);
    //    QCOMPARE(media.audioStream()->getDuration(), media.getDuration());

    QVERIFY(media.videoStream() != Q_NULLPTR);
    QCOMPARE(media.videoStream()->resolution(), videoResolution);
    QCOMPARE(QVariant::fromValue(media.videoStream()->frameRate()).toString(), QVariant::fromValue(videoFrameRate).toString());
    //    QCOMPARE(media.videoStream()->getDuration(), media.getDuration());

    QCOMPARE(media.getAudioLanguages(), audioLanguages);
    QCOMPARE(media.getVideoLanguages(), videoLanguages);
    QCOMPARE(media.getSubtitleLanguages(), subtitleLanguages);

    QCOMPARE(media.metaData("title"), title);
    QCOMPARE(media.metaData("artist"), artist);
    QCOMPARE(media.metaData("album"), album);
    QCOMPARE(media.metaData("track"), track);
    QCOMPARE(media.metaData("disc"), disc);
    QCOMPARE(media.metaData("genre"), genre);
    QCOMPARE(media.metaData("TLEN"), TLEN);
    QCOMPARE(media.metaData("date"), date);

    QByteArray picture = media.getPicture();
    QCOMPARE(picture.size(), pictureSize);
    QFile fd(QString("%1_QFfmpegMedia.jpeg").arg(format));
    fd.open(QIODevice::ReadWrite);
    fd.write(picture);
    fd.close();

    picture.clear();
    picture = media.getPicture();
    QImage image;
    QCOMPARE(image.loadFromData(picture), hasPicture);
    if (hasPicture)
    {
        QVERIFY(image.format() == QImage::Format_RGB32);
        QCOMPARE(image.width(), 300);
        QCOMPARE(image.height(), 300);
    }
}

void ffmpegTest::testQFfmpegMediaEncodeFileWAV_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<QString>("encoded_filename");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 28104126 << QString("Audio_MP3_Phebus");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 33328974 << QString("Audio_M4A_Monde_Virtuel");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 52874318 << QString("Audio_M4A_Daft_Punk");
}

void ffmpegTest::testQFfmpegMediaEncodeFileWAV()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(QString, encoded_filename);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QList<AVMediaType> l_mediaType;
    l_mediaType << AVMEDIA_TYPE_AUDIO;
    l_mediaType << AVMEDIA_TYPE_VIDEO;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openFile(QString("%1_%2.wav").arg(media.getFormat(), encoded_filename), l_mediaType), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("wav"));
    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16), true);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.context()->pb->pos, 78);  /* header written */

    QCOMPARE(outputMedia.filename(), QString("%1_%2.wav").arg(media.getFormat(), encoded_filename));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("wav"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.getBitrate(), 1411200);
    else
        QCOMPARE(outputMedia.getBitrate(), 1536000);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("pcm_s16le"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1411200);
    else
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1536000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    QCOMPARE(outputMedia.encodeMedia(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());

    QString filename = outputMedia.filename();
    outputMedia.close();
    QFileInfo file(filename);
    QCOMPARE(file.size(), encoded_size);
}

void ffmpegTest::testQFfmpegMediaEncodeFileMP3_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<QString>("encoded_filename");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 6374966 << QString("Audio_MP3_Phebus");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 7559880 << QString("Audio_M4A_Monde_Virtuel");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 11017965 << QString("Audio_M4A_Daft_Punk");
}

void ffmpegTest::testQFfmpegMediaEncodeFileMP3()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(QString, encoded_filename);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QList<AVMediaType> l_mediaType;
    l_mediaType << AVMEDIA_TYPE_AUDIO;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openFile(QString("%1_%2.mp3").arg(media.getFormat(), encoded_filename), l_mediaType), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("mp3"));

    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16P), true);
    QCOMPARE(outputMedia.audioStream()->setBitRate(320000), true);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.context()->pb->pos, 0);  /* header written */

    QCOMPARE(outputMedia.filename(), QString("%1_%2.mp3").arg(media.getFormat(), encoded_filename));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("mp3"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    QCOMPARE(outputMedia.getBitrate(), 320000);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("mp3"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16P);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->bitrate(), 320000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    QCOMPARE(outputMedia.encodeMedia(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    qInfo() << "size encoded" << encoded_size << "expected" << outputMedia.size();
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());

    // tests output file encoded
    QString filename = outputMedia.filename();
    outputMedia.close();

    QFfmpegInputMedia encodedMedia;
    QCOMPARE(encodedMedia.open(filename, true), true);
    QCOMPARE(encodedMedia.size(), encoded_size);
    encodedMedia.close();

    QFileInfo file(filename);
    QCOMPARE(file.size(), encoded_size);
}

void ffmpegTest::testQFfmpegMediaEncodeFileMP3WithImage_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<QString>("encoded_filename");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 6426017 << QString("AudioWithImage_MP3_Phebus");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 7796754 << QString("AudioWithImage_M4A_Monde_Virtuel");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 11082927 << QString("AudioWithImage_M4A_Daft_Punk");
}

void ffmpegTest::testQFfmpegMediaEncodeFileMP3WithImage()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(QString, encoded_filename);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QList<AVMediaType> l_mediaType;
    l_mediaType << AVMEDIA_TYPE_AUDIO;
    l_mediaType << AVMEDIA_TYPE_VIDEO;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openFile(QString("%1_%2.mp3").arg(media.getFormat(), encoded_filename), l_mediaType), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QVERIFY(outputMedia.videoStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("mp3"));

    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16P), true);
    QCOMPARE(outputMedia.audioStream()->setBitRate(320000), true);

    QCOMPARE(outputMedia.videoStream()->setPixelFormat(AV_PIX_FMT_RGB24), true);
    outputMedia.videoStream()->codec()->codecCtx()->width = 300;
    outputMedia.videoStream()->codec()->codecCtx()->height = 300;

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.videoStream()->isValid(), true);

    QCOMPARE(outputMedia.context()->pb->pos, 0);  /* header written */

    QCOMPARE(outputMedia.filename(), QString("%1_%2.mp3").arg(media.getFormat(), encoded_filename));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("mp3"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    QCOMPARE(outputMedia.getBitrate(), 320000);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("mp3"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16P);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->bitrate(), 320000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.videoStream()->format(), QString("png"));
    QCOMPARE(outputMedia.videoStream()->pixelFormat(), AV_PIX_FMT_RGB24);
    QCOMPARE(outputMedia.videoStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.videoStream()->stream()->time_base.den, 25);

    timer.start();

    QCOMPARE(outputMedia.encodeMedia(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());

    QString filename = outputMedia.filename();
    outputMedia.close();
    QFileInfo file(filename);
    QCOMPARE(file.size(), encoded_size);
}

void ffmpegTest::testQFfmpegMediaEncodeBufferWAV_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");


    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 28104126;
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 33328974;
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 52874318;
}

void ffmpegTest::testQFfmpegMediaEncodeBufferWAV()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QHash<AVMediaType, AVCodecID> mediaConfig;
    mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_NONE;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openBuffer("wav", mediaConfig), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("wav"));
    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16), true);
    QCOMPARE(outputMedia.audioStream()->setChannelLayout(AV_CH_LAYOUT_STEREO), true);
    QCOMPARE(outputMedia.bytesAvailable(), -1);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.bytesAvailable(), 78);  /* header written */

    QCOMPARE(outputMedia.filename(), QString(""));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("wav"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.getBitrate(), 1411200);
    else
        QCOMPARE(outputMedia.getBitrate(), 1536000);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("pcm_s16le"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1411200);
    else
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1536000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    qint64 bytesEncoded = 0;
    while (!outputMedia.atEnd())
    {
        QByteArray data = outputMedia.read(1000000);
        bytesEncoded += data.size();
        QVERIFY(data.size() <= 1000000);

        if (data.size() == 0)
            break;
    }

    QCOMPARE(outputMedia.bytesAvailable(), 0);
    QCOMPARE(bytesEncoded, encoded_size);
    QCOMPARE(outputMedia.atEnd(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());
}

void ffmpegTest::testQFfmpegMediaEncodeBufferMP3_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<int>("header_size");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 6374966 << 1089;
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 7559880 << 1089;
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 11017965 << 1005;
}

void ffmpegTest::testQFfmpegMediaEncodeBufferMP3()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(int, header_size);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QHash<AVMediaType, AVCodecID> mediaConfig;
    mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_MP3;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openBuffer("mp3", mediaConfig), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("mp3"));

    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16P), true);
    QCOMPARE(outputMedia.audioStream()->setBitRate(320000), true);

    QCOMPARE(outputMedia.bytesAvailable(), -1);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.bytesAvailable(), header_size);  /* header written */

    QCOMPARE(outputMedia.filename(), QString(""));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("mp3"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    QCOMPARE(outputMedia.getBitrate(), 320000);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("mp3"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16P);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->bitrate(), 320000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    qint64 bytesEncoded = 0;
    while (!outputMedia.atEnd())
    {
        QByteArray data = outputMedia.read(1000000);
        bytesEncoded += data.size();
        QVERIFY(data.size() <= 1000000);

        if (data.size() == 0)
            break;
    }

    QCOMPARE(outputMedia.bytesAvailable(), 0);
    QCOMPARE(bytesEncoded, encoded_size);
    QCOMPARE(outputMedia.atEnd(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());
}

void ffmpegTest::testQFfmpegMediaEncodeFileAAC_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<QString>("encoded_filename");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 2593525 << QString("Audio_MP3_Phebus");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 3034329 << QString("Audio_M4A_Monde_Virtuel");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 4494029 << QString("Audio_M4A_Daft_Punk");
}

void ffmpegTest::testQFfmpegMediaEncodeFileAAC()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(QString, encoded_filename);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QHash<AVMediaType, AVCodecID> mediaConfig;
    mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_AAC;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openFile(QString("%1_%2.m4a").arg(media.getFormat(), encoded_filename), QString("ipod"), mediaConfig), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));
    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_FLTP), true);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.context()->pb->pos, 40);  /* header written */

    QCOMPARE(outputMedia.filename(), QString("%1_%2.m4a").arg(media.getFormat(), encoded_filename));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    QCOMPARE(outputMedia.getBitrate(), 131840);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("aac"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_FLTP);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->bitrate(), 128000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    QCOMPARE(outputMedia.encodeMedia(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.audioStream()->format() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";

    QString filename = outputMedia.filename();
    qint64 outputSize = outputMedia.size();
    outputMedia.close();
    QFileInfo file(filename);
    QCOMPARE(file.size(), encoded_size);

    QVERIFY2(encoded_size < outputSize, QString("%1 %2").arg(encoded_size).arg(outputSize).toUtf8());
}

void ffmpegTest::testQFfmpegMediaEncodeBufferAAC_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<int>("header_size");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 2565278 << 40;
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 3000982 << 40;
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 4441562 << 40;
}

void ffmpegTest::testQFfmpegMediaEncodeBufferAAC()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(int, header_size);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QHash<AVMediaType, AVCodecID> mediaConfig;
    mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_AAC;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openBuffer("ipod", mediaConfig), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));

    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_FLTP), true);
    QCOMPARE(outputMedia.audioStream()->setBitRate(128000), true);

    QCOMPARE(outputMedia.bytesAvailable(), -1);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.bytesAvailable(), header_size);  /* header written */

    QCOMPARE(outputMedia.filename(), QString(""));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    QCOMPARE(outputMedia.getBitrate(), 131840);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("aac"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_FLTP);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->bitrate(), 128000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    qint64 bytesEncoded = 0;
    while (!outputMedia.atEnd())
    {
        QByteArray data = outputMedia.read(1000000);
        bytesEncoded += data.size();
        QVERIFY(data.size() <= 1000000);

        if (data.size() == 0)
            break;
    }

    QCOMPARE(outputMedia.bytesAvailable(), 0);
    QCOMPARE(bytesEncoded, encoded_size);
    QCOMPARE(outputMedia.atEnd(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());
}

void ffmpegTest::testQFfmpegMediaEncodeFileLPCM_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<QString>("encoded_filename");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 28104048 << QString("Audio_MP3_Phebus");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 33328896 << QString("Audio_M4A_Monde_Virtuel");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 52874240 << QString("Audio_M4A_Daft_Punk");
}

void ffmpegTest::testQFfmpegMediaEncodeFileLPCM()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(QString, encoded_filename);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QList<AVMediaType> l_mediaType;
    l_mediaType << AVMEDIA_TYPE_AUDIO;
    l_mediaType << AVMEDIA_TYPE_VIDEO;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openFile(QString("%1_%2.pcm").arg(media.getFormat(), encoded_filename), QString("s16le"), l_mediaType), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("s16le"));
    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16), true);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.context()->pb->pos, 0);  /* header written */

    QCOMPARE(outputMedia.filename(), QString("%1_%2.pcm").arg(media.getFormat(), encoded_filename));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("s16le"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.getBitrate(), 1411200);
    else
        QCOMPARE(outputMedia.getBitrate(), 1536000);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("pcm_s16le"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1411200);
    else
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1536000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    QCOMPARE(outputMedia.encodeMedia(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());

    QString filename = outputMedia.filename();
    outputMedia.close();
    QFileInfo file(filename);
    QCOMPARE(file.size(), encoded_size);
}

void ffmpegTest::testQFfmpegMediaEncodeBufferLPCM_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");


    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 28104048;
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 33328896;
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 52874240;
}

void ffmpegTest::testQFfmpegMediaEncodeBufferLPCM()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QHash<AVMediaType, AVCodecID> mediaConfig;
    mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_PCM_S16LE;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openBuffer("s16le", mediaConfig), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("s16le"));
    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16), true);
    QCOMPARE(outputMedia.audioStream()->setChannelLayout(AV_CH_LAYOUT_STEREO), true);
    QCOMPARE(outputMedia.bytesAvailable(), -1);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.bytesAvailable(), 0);  /* header written */

    QCOMPARE(outputMedia.filename(), QString(""));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("s16le"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.getBitrate(), 1411200);
    else
        QCOMPARE(outputMedia.getBitrate(), 1536000);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("pcm_s16le"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    if (media.audioStream()->samplerate() == 44100)
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1411200);
    else
        QCOMPARE(outputMedia.audioStream()->bitrate(), 1536000);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    qint64 bytesEncoded = 0;
    while (!outputMedia.atEnd())
    {
        QByteArray data = outputMedia.read(1000000);
        bytesEncoded += data.size();
        QVERIFY(data.size() <= 1000000);

        if (data.size() == 0)
            break;
    }

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";

    QCOMPARE(outputMedia.bytesAvailable(), 0);
    QCOMPARE(bytesEncoded, encoded_size);
    QCOMPARE(outputMedia.atEnd(), true);

    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());
}

void ffmpegTest::testQFfmpegMediaEncodeFileALAC_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");
    QTest::addColumn<QString>("encoded_filename");

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 15593061 << QString("Audio_MP3_Phebus");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 21335483 << QString("Audio_M4A_Monde_Virtuel");
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 33189737 << QString("Audio_M4A_Daft_Punk");
}

void ffmpegTest::testQFfmpegMediaEncodeFileALAC()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);
    QFETCH(QString, encoded_filename);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QHash<AVMediaType, AVCodecID> mediaConfig;
    mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_ALAC;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openFile(QString("%1_%2.alac").arg(media.getFormat(), encoded_filename), QString("ipod"), mediaConfig), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));
    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16P), true);
    outputMedia.audioStream()->setBitRate(1411200);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.context()->pb->pos, 40);  /* header written */

    QCOMPARE(outputMedia.filename(), QString("%1_%2.alac").arg(media.getFormat(), encoded_filename));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    QCOMPARE(outputMedia.getBitrate(), 1453536);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("alac"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16P);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->bitrate(), 1411200);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    QCOMPARE(outputMedia.encodeMedia(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.audioStream()->format() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());

    QString filename = outputMedia.filename();
    outputMedia.close();
    QFileInfo file(filename);
    QCOMPARE(file.size(), encoded_size);
}

void ffmpegTest::testQFfmpegMediaEncodeBufferALAC_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("duration");
    QTest::addColumn<int>("encoded_size");


    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373061 << 15585306;
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987211 << 21326356;
    QTest::newRow("Audio_M4A_2") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/Daft Punk/Random Access Memories/01 Give Life Back To Music.m4a") << 275387000 << 33175662;
}

void ffmpegTest::testQFfmpegMediaEncodeBufferALAC()
{
    QElapsedTimer timer;

    QFETCH(QString, pathname);
    QFETCH(int, duration);
    QFETCH(int, encoded_size);

    timer.start();
    QFfmpegInputMedia media;
    QCOMPARE(media.open(pathname, true), true);
    qInfo() << "media initialised in" << timer.elapsed() << "ms.";
    QVERIFY2(timer.elapsed() < 100, QString("media initialised in %1 ms.").arg(timer.elapsed()).toUtf8());
    qInfo() << "media duration:" << media.getDurationInMicroSec() << "us";

    /* initialise output */
    QHash<AVMediaType, AVCodecID> mediaConfig;
    mediaConfig[AVMEDIA_TYPE_AUDIO] = AV_CODEC_ID_ALAC;

    QFfmpegOutputMedia outputMedia;
    QCOMPARE(outputMedia.openBuffer("ipod", mediaConfig), true);
    QCOMPARE(outputMedia.isValid(), true);
    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));
    QCOMPARE(outputMedia.audioStream()->setSampleFmt(AV_SAMPLE_FMT_S16P), true);
    QCOMPARE(outputMedia.audioStream()->setChannelLayout(AV_CH_LAYOUT_STEREO), true);
    outputMedia.audioStream()->setBitRate(1411200);
    QCOMPARE(outputMedia.bytesAvailable(), -1);

    QCOMPARE(outputMedia.setInputMedia(&media), true);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);

    QCOMPARE(outputMedia.bytesAvailable(), 40);  /* header written */

    QCOMPARE(outputMedia.filename(), QString(""));
    QCOMPARE(outputMedia.isValid(), true);
    QCOMPARE(outputMedia.getFormat(), QString("ipod"));
    QCOMPARE(outputMedia.getDurationInMicroSec(), duration);
    QCOMPARE(outputMedia.getBitrate(), 1453536);

    QVERIFY(outputMedia.audioStream() != Q_NULLPTR);
    QCOMPARE(outputMedia.audioStream()->isValid(), true);
    QCOMPARE(outputMedia.audioStream()->format(), QString("alac"));
    QCOMPARE(outputMedia.audioStream()->sampleFormat(), AV_SAMPLE_FMT_S16P);
    QCOMPARE(outputMedia.audioStream()->samplerate(), media.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->channelCount(), 2);
    QCOMPARE((int)outputMedia.audioStream()->channelLayout(), (int)AV_CH_LAYOUT_STEREO);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.num, 1);
    QCOMPARE(outputMedia.audioStream()->stream()->time_base.den, outputMedia.audioStream()->samplerate());
    QCOMPARE(outputMedia.audioStream()->bitrate(), 1411200);
    QCOMPARE(outputMedia.audioStream()->getDurationInMicroSec()/10000, duration/10000);

    QVERIFY(outputMedia.videoStream() == Q_NULLPTR);

    timer.start();

    qint64 bytesEncoded = 0;
    while (!outputMedia.atEnd())
    {
        QByteArray data = outputMedia.read(1000000);
        bytesEncoded += data.size();
        QVERIFY(data.size() <= 1000000);

        if (data.size() == 0)
            break;
    }

    QCOMPARE(outputMedia.bytesAvailable(), 0);
    QCOMPARE(bytesEncoded, encoded_size);
    QCOMPARE(outputMedia.atEnd(), true);

    qInfo() << "input audio stream duration:" << media.audioStream()->getDurationInMicroSec() << "us, audio encoded in " << outputMedia.getFormat() << ", output audio stream duration:" << outputMedia.posInMsec() << "ms, in" << timer.elapsed() << "ms.";
    QVERIFY2(encoded_size < outputMedia.size(), QString("%1 %2").arg(encoded_size).arg(outputMedia.size()).toUtf8());
}

QTEST_APPLESS_MAIN(ffmpegTest)

#include "tst_ffmpegtest.moc"

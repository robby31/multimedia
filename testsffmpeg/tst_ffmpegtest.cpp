#include <QString>
#include <QtTest>

#include "qffmpeg.h"

class ffmpegTest : public QObject
{
    Q_OBJECT

public:
    ffmpegTest();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();
};

ffmpegTest::ffmpegTest()
{
    QFfmpeg::setDirPath("/opt/local/bin");
}

void ffmpegTest::testCase1_data()
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

    QTest::newRow("Audio_MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << QString("mp3") << 159373 << "mp3" << 2 << 44100 << 189255 << 192808 << QString("500x500") << 0.0 << QStringList("") << QStringList("") << QStringList() << 5290 << QString("Phébus") << QString("-M-") << QString("Mister Mystère") << QString("2/13") << QString("1/2") << QString("Rock  Français") << QString("159320") << QString("2009-09-07");
    QTest::newRow("Audio_M4A") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << QString("mov,mp4,m4a,3gp,3g2,mj2") << 188987 << "aac" << 2 << 44100 << 124436 << 135074 << QString("300x300") << 0.0 << QStringList("") << QStringList("und") << QStringList() << 21247 << QString("Monde virtuel") << QString("-M-") << QString("Je dis aime") << QString("1/15") << QString("1/1") << QString("Pop") << QString("") << QString("1999-10-25");
    QTest::newRow("Video_MKV") << QString("/Users/doudou/Movies/Films/District.9.2009.720p.BrRip.YIFY.mkv") << QString("matroska,webm") << 6735830 << "aac" << 2 << 48000 << 0 << 871427 << QString("1280x688") << 23.976023976023978 << QStringList("") << QStringList("") << QStringList("eng") << 2008 << QString("District 9 - YIFY") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("");

    QStringList tmp;
    tmp << "spa" << "eng" << "fre";
    QTest::newRow("Video_MKV2") << QString("/Users/doudou/Movies/Films/Margin Call [BDrip m-1080p AC3 Esp-Eng-Fr+subs].mkv") << QString("matroska,webm") << 6419022 << "ac3" << 2 << 48000 << 112000 << 4293543 << QString("1920x1080") << 24.0 << QStringList("spa") << QStringList(tmp) << QStringList(tmp) << 4054 << QString("Margin Call m-1080p.guerrero36") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("") << QString("");

}

void ffmpegTest::testCase1()
{
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

    QFfmpeg media(pathname);

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

    QByteArray picture = media.getPicture();
    QCOMPARE(picture.size(), pictureSize);

    QCOMPARE(media.metaData("title"), title);
    QCOMPARE(media.metaData("artist"), artist);
    QCOMPARE(media.metaData("album"), album);
    QCOMPARE(media.metaData("track"), track);
    QCOMPARE(media.metaData("disc"), disc);
    QCOMPARE(media.metaData("genre"), genre);
    QCOMPARE(media.metaData("TLEN"), TLEN);
    QCOMPARE(media.metaData("date"), date);

    ANALYZER_DISPLAY_RESULTS
}

QTEST_APPLESS_MAIN(ffmpegTest)

#include "tst_ffmpegtest.moc"

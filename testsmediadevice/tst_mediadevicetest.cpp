#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include "streamingfile.h"
#include "ffmpegtranscoding.h"
#include "wavfile.h"
#include "qffmpegtranscoding.h"

class mediadeviceTest : public QObject
{
    Q_OBJECT

public:
    mediadeviceTest();

private Q_SLOTS:
    void cleanup();

    void testQFfmpegTranscodingALAC_data();
    void testQFfmpegTranscodingALAC();

    void testQFfmpegTranscodingLPCM_data();
    void testQFfmpegTranscodingLPCM();

    void testQFfmpegTranscodingAAC_data();
    void testQFfmpegTranscodingAAC();

    void testQFfmpegTranscodingWAV_data();
    void testQFfmpegTranscodingWAV();

    void testQFfmpegTranscodingMP3_data();
    void testQFfmpegTranscodingMP3();

    void testWavFile();
    void testTranscodedWavFileFromMP3();
    void testTranscodedWavFileFromM4A();

    void testStreamingFile_data();
    void testStreamingFile();

    void testFfmpegTranscoding_data();
    void testFfmpegTranscoding();

    void testQFfmpegTranscodingCOPY_data();
    void testQFfmpegTranscodingCOPY();
};

mediadeviceTest::mediadeviceTest()
{
    FfmpegTranscoding::setDirPath("/usr/local/bin");
}

void mediadeviceTest::cleanup()
{
    DebugInfo::display_alive_objects();

    QCOMPARE(DebugInfo::count_alive_objects(), 0);
}

void mediadeviceTest::testStreamingFile_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::newRow("MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 3841064 << 10485760 << 20 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 RANGE=3000-") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 3841064-3000 << 10485760 << 20 << 3000 << -1 << 3000 << -1;
    QTest::newRow("MP3 invalid startByte") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 3841064 << 10485760 << 20 << -3 << -1 << -1 << -1;
    QTest::newRow("MP3 RANGE=0-5000") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 5001 << 10485760 << 20 << 0 << 5000 << 0 << 5000;
    QTest::newRow("MP3 invalid endByte") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 3841064 << 10485760 << 20 << 0 << -5 << 0 << -1;
    QTest::newRow("MP3 RANGE=1000-5000") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 4001 << 10485760 << 20 << 1000 << 5000 << 1000 << 5000;
    QTest::newRow("MP3 invalid RANGE startByte > endByte") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 3841064-3000 << 10485760 << 20 << 3000 << 1000 << 3000 << -1;
    QTest::newRow("MP3 invalid RANGE startByte > size") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 3841064 << 10485760 << 20 << 4841064 << -1 << 0 << -1;
    QTest::newRow("MP3 invalid RANGE endByte > size") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 3841064 << 10485760 << 20 << 0 << 4841064 << 0 << 3841063;
}

void mediadeviceTest::testStreamingFile()
{
    QFETCH(QString, pathname);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);

    StreamingFile device(pathname);

    device.setRange(param_startByte, param_endByte);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), size);

    QCOMPARE(device.timeSeekStart(), -1);
    QCOMPARE(device.timeSeekEnd(), -1);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), -1);

    /* OPEN DEVICE */
    QCOMPARE(device.open(), true);
    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), size);

    QCOMPARE(device.timeSeekStart(), -1);
    QCOMPARE(device.timeSeekEnd(), -1);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), -1);

    /* READ DEVICE UNTIL END */
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000);
        readBytes += data.size();
    }
    QCOMPARE(readBytes, size);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), size);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), -1);
    QCOMPARE(device.timeSeekEnd(), -1);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), -1);
}

void mediadeviceTest::testFfmpegTranscoding_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("param_durationInMSeconds");
    QTest::addColumn<int>("durationInMSeconds");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::addColumn<int>("param_time_start");
    QTest::addColumn<int>("time_start");
    QTest::addColumn<int>("param_time_end");
    QTest::addColumn<int>("time_end");

    QTest::newRow("MP3 bitrate=320000") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 159373 << 320000 << 6376920 << 800000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 149373 << 320000 << 5976920 << 800000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("MP3 TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 10000 << 320000 << 402000 << 800000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("MP3 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 30000 << 320000 << 1202000 << 800000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("MP3 TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 20000 << 320000 << 802000 << 800000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;

    QTest::newRow("MP3 bitrate=192000") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 159373 << 192000 << 3826952 << 480000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
}

void mediadeviceTest::testFfmpegTranscoding()
{
    QFETCH(QString, pathname);
    QFETCH(int, param_durationInMSeconds);
    QFETCH(int, durationInMSeconds);
    QFETCH(int, bitrate);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);
    QFETCH(int, param_time_start);
    QFETCH(int, time_start);
    QFETCH(int, param_time_end);
    QFETCH(int, time_end);

    FfmpegTranscoding device;
    device.setUrl(pathname);
    device.setOriginalLengthInMSeconds(param_durationInMSeconds);
    device.setFormat(MP3);
    device.setBitrate(bitrate);

    if (param_startByte != -1 or param_endByte != -1)
        device.setRange(param_startByte, param_endByte);
    if (param_time_start != -1 or param_time_end != -1)
        device.setTimeSeek(param_time_start, param_time_end);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == MP3, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    QElapsedTimer timer;

    /* OPEN DEVICE */
    timer.start();
    QCOMPARE(device.open(), true);
    device.waitForFinished(-1);
    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* READ DEVICE UNTIL END */
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000);
        readBytes += data.size();

    }
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);
}

void mediadeviceTest::testWavFile()
{
    WavFile wav;
    QCOMPARE(wav.openLocalFile("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/test.wav"), true);
    QCOMPARE(wav.error(), WavFile::NoError);
    QCOMPARE(wav.headerLength(), 44);
    QCOMPARE(wav.fileFormat().byteOrder(), QAudioFormat::LittleEndian);
    QCOMPARE(wav.fileFormat().channelCount(), 2);
    QCOMPARE(wav.fileFormat().codec(), QString("audio/pcm"));
    QCOMPARE(wav.fileFormat().sampleRate(), 48000);
    QCOMPARE(wav.fileFormat().sampleSize(), 16);
    QCOMPARE(wav.fileFormat().sampleType(), QAudioFormat::SignedInt);

    QCOMPARE(wav.bytesPerSample(), 4);
    QCOMPARE(wav.bitrate(), 1536000);
    QCOMPARE(wav.samplesAvailable(), 26142080);
    QCOMPARE(wav.bytesAvailable(), 104568320);

    QCOMPARE(wav.pos(), 44);
    QCOMPARE(wav.size(), 104568364);
    QCOMPARE(wav.durationMsec(), 544626);

    QCOMPARE(wav.audioDurationMsec(0), 0);
    QCOMPARE(wav.audioDurationMsec(192000), 1000);
    QCOMPARE(wav.audioDurationMsec(wav.size()-wav.headerLength()), wav.durationMsec());

    QCOMPARE(wav.audioLength(0), 0);
    QCOMPARE(wav.audioLength(1000), 192000);
    QCOMPARE(wav.audioLength(wav.durationMsec()), wav.size()-wav.headerLength()-128);
}

void mediadeviceTest::testTranscodedWavFileFromMP3()
{
    WavFile wav;
    QCOMPARE(wav.openLocalFile("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/07 On_Off.mp3"), true);
    QCOMPARE(wav.error(), WavFile::NoError);
    QCOMPARE(wav.headerLength(), 44);
    QCOMPARE(wav.fileFormat().byteOrder(), QAudioFormat::LittleEndian);
    QCOMPARE(wav.fileFormat().channelCount(), 2);
    QCOMPARE(wav.fileFormat().codec(), QString("audio/pcm"));
    QCOMPARE(wav.fileFormat().sampleRate(), 44100);
    QCOMPARE(wav.fileFormat().sampleSize(), 16);
    QCOMPARE(wav.fileFormat().sampleType(), QAudioFormat::SignedInt);

    QCOMPARE(wav.bytesPerSample(), 4);
    QCOMPARE(wav.bitrate(), 1411200);
    QCOMPARE(wav.samplesAvailable(), 852608);
    QCOMPARE(wav.bytesAvailable(), 3410434);

    QCOMPARE(wav.pos(), 44);
    QCOMPARE(wav.size(), 3410478);
    QCOMPARE(wav.durationMsec(), 19333);

    QCOMPARE(wav.audioDurationMsec(0), 0);
    QCOMPARE(wav.audioDurationMsec(176384), 999);
    QCOMPARE(wav.audioDurationMsec(wav.size()-wav.headerLength()), wav.durationMsec());

    QCOMPARE(wav.audioLength(0), 0);
    QCOMPARE(wav.audioLength(1000), 176384);
    QCOMPARE(wav.audioLength(wav.durationMsec()), wav.size()-wav.headerLength()-98);
}

void mediadeviceTest::testTranscodedWavFileFromM4A()
{
    WavFile wav;
    QCOMPARE(wav.openLocalFile("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a"), true);
    QCOMPARE(wav.error(), WavFile::NoError);
    QCOMPARE(wav.headerLength(), 44);
    QCOMPARE(wav.fileFormat().byteOrder(), QAudioFormat::LittleEndian);
    QCOMPARE(wav.fileFormat().channelCount(), 2);
    QCOMPARE(wav.fileFormat().codec(), QString("audio/pcm"));
    QCOMPARE(wav.fileFormat().sampleRate(), 44100);
    QCOMPARE(wav.fileFormat().sampleSize(), 16);
    QCOMPARE(wav.fileFormat().sampleType(), QAudioFormat::SignedInt);

    QCOMPARE(wav.bytesPerSample(), 4);
    QCOMPARE(wav.bitrate(), 1411200);
    QCOMPARE(wav.samplesAvailable(), 8332232);
    QCOMPARE(wav.bytesAvailable(), 33328930);

    QCOMPARE(wav.pos(), 44);
    QCOMPARE(wav.size(), 33328974);
    QCOMPARE(wav.durationMsec(), 188939);

    QCOMPARE(wav.audioDurationMsec(0), 0);
    QCOMPARE(wav.audioDurationMsec(176384), 999);
    QCOMPARE(wav.audioDurationMsec(wav.size()-wav.headerLength()), wav.durationMsec());

    QCOMPARE(wav.audioLength(0), 0);
    QCOMPARE(wav.audioLength(1000), 176384);
    QCOMPARE(wav.audioLength(wav.durationMsec()), wav.size()-wav.headerLength()-98);
}

void mediadeviceTest::testQFfmpegTranscodingMP3_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("durationInMSeconds");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::addColumn<int>("param_time_start");
    QTest::addColumn<int>("time_start");
    QTest::addColumn<int>("param_time_end");
    QTest::addColumn<int>("time_end");

    QTest::newRow("MP3 bitrate=320000") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 320000 << 6376920 << 800000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 bitrate=320000 TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 149373 << 320000 << 5976920 << 800000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("MP3 bitrate=320000 TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 10000 << 320000 << 402000 << 800000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("MP3 bitrate=320000 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 30000 << 320000 << 1202000 << 800000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("MP3 bitrate=320000 TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 20000 << 320000 << 802000 << 800000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;

    QTest::newRow("MP3 bitrate=192000") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 192000 << 3826952 << 480000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 bitrate=192000 TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 149373 << 192000 << 3586952 << 480000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("MP3 bitrate=192000 TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 10000 << 192000 << 242000 << 480000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("MP3 bitrate=192000 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 30000 << 192000 << 722000 << 480000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("MP3 bitrate=192000 TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 20000 << 192000 << 482000 << 480000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;
}

void mediadeviceTest::testQFfmpegTranscodingMP3()
{
    QFETCH(QString, pathname);
    QFETCH(int, durationInMSeconds);
    QFETCH(int, bitrate);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);
    QFETCH(int, param_time_start);
    QFETCH(int, time_start);
    QFETCH(int, param_time_end);
    QFETCH(int, time_end);

    QFfmpegTranscoding device;
    device.setUrl(pathname);
    device.setFormat(MP3);
    device.setBitrate(bitrate);

    if (param_startByte != -1 or param_endByte != -1)
        device.setRange(param_startByte, param_endByte);
    if (param_time_start != -1 or param_time_end != -1)
        device.setTimeSeek(param_time_start, param_time_end);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == MP3, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    QElapsedTimer timer;

    /* READ DEVICE UNTIL END */
    timer.start();
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    device.close();
    /*
          transcode second time after closing device
    */

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == MP3, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE  */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* READ DEVICE UNTIL END */
    timer.start();
    readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

}

void mediadeviceTest::testQFfmpegTranscodingWAV_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("durationInMSeconds");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::addColumn<int>("param_time_start");
    QTest::addColumn<int>("time_start");
    QTest::addColumn<int>("param_time_end");
    QTest::addColumn<int>("time_end");

    QTest::newRow("MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 1411200 << 28113397 << 3528000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 149373 << 1411200 << 26349397 << 3528000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("MP3 TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 10000 << 1411200 << 1764000 << 3528000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("MP3 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 30000 << 1411200 << 5292000 << 3528000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("MP3 TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 20000 << 1411200 << 3528000 << 3528000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;
}

void mediadeviceTest::testQFfmpegTranscodingWAV()
{
    QFETCH(QString, pathname);
    QFETCH(int, durationInMSeconds);
    QFETCH(int, bitrate);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);
    QFETCH(int, param_time_start);
    QFETCH(int, time_start);
    QFETCH(int, param_time_end);
    QFETCH(int, time_end);

    QFfmpegTranscoding device;
    device.setFormat(WAV);
    device.setUrl(pathname);

    if (param_startByte != -1 or param_endByte != -1)
        device.setRange(param_startByte, param_endByte);
    if (param_time_start != -1 or param_time_end != -1)
        device.setTimeSeek(param_time_start, param_time_end);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == WAV, true);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    QElapsedTimer timer;

    /* READ DEVICE UNTIL END */
    timer.start();
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QVERIFY2(readBytes<=size, QString("%1 > %2").arg(readBytes).arg(size).toUtf8());

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    device.close();
    /*
          transcode second time after closing device
    */

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == WAV, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE  */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* READ DEVICE UNTIL END */
    timer.start();
    readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

}

void mediadeviceTest::testQFfmpegTranscodingAAC_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("durationInMSeconds");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<int>("audioBitrate");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::addColumn<int>("param_time_start");
    QTest::addColumn<int>("time_start");
    QTest::addColumn<int>("param_time_end");
    QTest::addColumn<int>("time_end");

    QTest::newRow("MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 329600 << 320000 << 6631829 << 824000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 149373 << 329600 << 320000 << 6215709 << 824000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("MP3 TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 10000 << 329600 << 320000 << 416120 << 824000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("MP3 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 30000 << 329600 << 320000 << 1248360 << 824000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("MP3 TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 20000 << 329600 << 320000 << 832240 << 824000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;
}

void mediadeviceTest::testQFfmpegTranscodingAAC()
{
    QFETCH(QString, pathname);
    QFETCH(int, durationInMSeconds);
    QFETCH(int, bitrate);
    QFETCH(int, audioBitrate);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);
    QFETCH(int, param_time_start);
    QFETCH(int, time_start);
    QFETCH(int, param_time_end);
    QFETCH(int, time_end);

    QFfmpegTranscoding device;
    device.setFormat(AAC);
    device.setUrl(pathname);
    device.setBitrate(audioBitrate);

    if (param_startByte != -1 or param_endByte != -1)
        device.setRange(param_startByte, param_endByte);
    if (param_time_start != -1 or param_time_end != -1)
        device.setTimeSeek(param_time_start, param_time_end);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == AAC, true);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);  /* header written */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    QElapsedTimer timer;

    /* READ DEVICE UNTIL END */
    timer.start();
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QVERIFY2(readBytes<=size, QString("%1 > %2").arg(readBytes).arg(size).toUtf8());

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    device.close();
    /*
          transcode second time after closing device
    */

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == AAC, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE  */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);  /* header  written */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* READ DEVICE UNTIL END */
    timer.start();
    readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

}

void mediadeviceTest::testQFfmpegTranscodingLPCM_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("durationInMSeconds");
    QTest::addColumn<int>("decoded_durationInMSeconds");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::addColumn<int>("param_time_start");
    QTest::addColumn<int>("time_start");
    QTest::addColumn<int>("param_time_end");
    QTest::addColumn<int>("time_end");

    QTest::newRow("MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 159320 << 1411200 << 28113678 << 3528000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 149373 << 149340 << 1411200 << 26349660 << 3528000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("MP3 TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 10000 << 9953 << 1411200 << 1764017 << 3528000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("MP3 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 30000 << 29963 << 1411200 << 5292052 << 3528000 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("MP3 TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 20000 << 19983 << 1411200 << 3528035 << 3528000 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;
}

void mediadeviceTest::testQFfmpegTranscodingLPCM()
{
    QFETCH(QString, pathname);
    QFETCH(int, durationInMSeconds);
    QFETCH(int, decoded_durationInMSeconds);
    QFETCH(int, bitrate);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);
    QFETCH(int, param_time_start);
    QFETCH(int, time_start);
    QFETCH(int, param_time_end);
    QFETCH(int, time_end);

    QFfmpegTranscoding device;
    device.setFormat(LPCM_S16BE);
    device.setUrl(pathname);

    if (param_startByte != -1 or param_endByte != -1)
        device.setRange(param_startByte, param_endByte);
    if (param_time_start != -1 or param_time_end != -1)
        device.setTimeSeek(param_time_start, param_time_end);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == LPCM_S16BE, true);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() == 0, true);  /* no header */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    QElapsedTimer timer;

    /* READ DEVICE UNTIL END */
    timer.start();
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(device.posInMsec(), decoded_durationInMSeconds);
    QVERIFY2(readBytes<=size, QString("%1 > %2").arg(readBytes).arg(size).toUtf8());

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    device.close();
    /*
          transcode second time after closing device
    */

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == LPCM_S16BE, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE  */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() == 0, true);  /* no header */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* READ DEVICE UNTIL END */
    timer.start();
    readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

}

void mediadeviceTest::testQFfmpegTranscodingALAC_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("durationInMSeconds");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::addColumn<int>("param_time_start");
    QTest::addColumn<int>("time_start");
    QTest::addColumn<int>("param_time_end");
    QTest::addColumn<int>("time_end");

    QTest::newRow("MP3") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 1453536 << 31273343 << 3633840 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 149373 << 1453536 << 29311069 << 3633840 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("MP3 TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 10000 << 1453536 << 1962273 << 3633840 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("MP3 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 30000 << 1453536 << 5886820 << 3633840 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("MP3 TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 20000 << 1453536 << 3924547 << 3633840 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;
}

void mediadeviceTest::testQFfmpegTranscodingALAC()
{
    QFETCH(QString, pathname);
    QFETCH(int, durationInMSeconds);
    QFETCH(int, bitrate);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);
    QFETCH(int, param_time_start);
    QFETCH(int, time_start);
    QFETCH(int, param_time_end);
    QFETCH(int, time_end);

    QFfmpegTranscoding device;
    device.setFormat(ALAC);
    device.setUrl(pathname);

    if (param_startByte != -1 or param_endByte != -1)
        device.setRange(param_startByte, param_endByte);
    if (param_time_start != -1 or param_time_end != -1)
        device.setTimeSeek(param_time_start, param_time_end);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == ALAC, true);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);  /* header written */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    QElapsedTimer timer;

    /* READ DEVICE UNTIL END */
    timer.start();
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QVERIFY2(readBytes<=size, QString("%1 > %2").arg(readBytes).arg(size).toUtf8());

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    device.close();
    /*
          transcode second time after closing device
    */

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == ALAC, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE  */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);  /* header written */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* READ DEVICE UNTIL END */
    timer.start();
    readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

}

void mediadeviceTest::testQFfmpegTranscodingCOPY_data()
{
    QTest::addColumn<QString>("pathname");
    QTest::addColumn<int>("durationInMSeconds");
    QTest::addColumn<int>("bitrate");
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("maxBufferSize");
    QTest::addColumn<int>("durationBuffer");
    QTest::addColumn<int>("param_startByte");
    QTest::addColumn<int>("param_endByte");
    QTest::addColumn<int>("startByte");
    QTest::addColumn<int>("endByte");
    QTest::addColumn<int>("param_time_start");
    QTest::addColumn<int>("time_start");
    QTest::addColumn<int>("param_time_end");
    QTest::addColumn<int>("time_end");

    QTest::newRow("COPY") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 189255 << 3770267 << 473120 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("COPY TimeStart=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 149373 << 189255 << 3533698 << 473120 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << -1 << -1;
    QTest::newRow("COPY TimeEnd=10") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 10000 << 189255 << 236568 << 473120 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << 10;
    QTest::newRow("COPY TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 30000 << 189255 << 709706 << 473120 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << 30 << 30;
    QTest::newRow("COPY TimeStart=10 TimeEnd=30") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 20000 << 189255 << 473137 << 473120 << 20 << -1 << -1 << -1 << -1 << 10 << 10 << 30 << 30;
    QTest::newRow("COPY RANGE=3000-") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 189255 << 3770267-3000 << 473120 << 20 << 3000 << -1 << 3000 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("COPY RANGE=0-5000") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 189255 << 5001 << 473120 << 20 << 0 << 5000 << 0 << 5000 << -1 << -1 << -1 << -1;

    QTest::newRow("COPY") << QString("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/01 Monde virtuel.m4a") << 188987 << 128169 << 3027784 << 320420 << 20 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1;
}

void mediadeviceTest::testQFfmpegTranscodingCOPY()
{
    QFETCH(QString, pathname);
    QFETCH(int, durationInMSeconds);
    QFETCH(int, bitrate);
    QFETCH(int, size);
    QFETCH(int, maxBufferSize);
    QFETCH(int, durationBuffer);
    QFETCH(int, param_startByte);
    QFETCH(int, param_endByte);
    QFETCH(int, startByte);
    QFETCH(int, endByte);
    QFETCH(int, param_time_start);
    QFETCH(int, time_start);
    QFETCH(int, param_time_end);
    QFETCH(int, time_end);

    QFfmpegTranscoding device;
    device.setFormat(COPY);
    device.setUrl(pathname);

    if (param_startByte != -1 or param_endByte != -1)
        device.setRange(param_startByte, param_endByte);
    if (param_time_start != -1 or param_time_end != -1)
        device.setTimeSeek(param_time_start, param_time_end);

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == COPY, true);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable() > 0, true);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    QElapsedTimer timer;

    /* READ DEVICE UNTIL END */
    timer.start();
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QVERIFY2(readBytes<=size, QString("%1 > %2").arg(readBytes).arg(size).toUtf8());

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    device.close();
    /*
          transcode second time after closing device
    */

    QCOMPARE(device.isOpen(), false);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);

    QCOMPARE(device.format() == COPY, true);
    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), -1);  /* device not open */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* OPEN DEVICE  */
    QCOMPARE(device.open(), true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), false);

    QCOMPARE(device.pos(), 0);
    QCOMPARE(device.progress(), 0);
    QCOMPARE(device.posInMsec(), 0);

    QCOMPARE(device.size(), size);
//    QCOMPARE(device.bytesAvailable() == 0, true);  /* no header written */

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

    /* READ DEVICE UNTIL END */
    timer.start();
    readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000000);
        readBytes += data.size();
        QVERIFY(data.size() <= 1000000);
    }

    qInfo() << "transcoding done in" << timer.elapsed() << "ms.";
    delta_size = (double)(size-readBytes)/(double)size*100.0;
    qInfo() << "delta size" << delta_size;
    qInfo() << "bytes read" << readBytes << size;
    //    if (durationInMSeconds > 20000)
    //        QCOMPARE(delta_size < 0.07, true);
    QCOMPARE(readBytes<=size, true);

    QCOMPARE(device.isOpen(), true);
    QCOMPARE(device.isReadyToOpen(), true);
    QCOMPARE(device.atEnd(), true);

    QCOMPARE(qRound(device.posInMsec()/1000.0), qRound(durationInMSeconds/1000.0));
    QCOMPARE(device.pos(), readBytes);
    QCOMPARE(device.progress(), 100);

    QCOMPARE(device.size(), size);
    QCOMPARE(device.bytesAvailable(), 0);

    QCOMPARE(device.timeSeekStart(), time_start);
    QCOMPARE(device.timeSeekEnd(), time_end);

    QCOMPARE(device.startByte(), startByte);
    QCOMPARE(device.endByte(), endByte);

    QCOMPARE(device.maxBufferSize(), maxBufferSize);
    QCOMPARE(device.durationBuffer(), durationBuffer);
    QCOMPARE(device.bitrate(), bitrate);
    QCOMPARE(device.lengthInMSeconds(), durationInMSeconds);
    QCOMPARE(device.lengthInSeconds(), durationInMSeconds/1000);

}
QTEST_MAIN(mediadeviceTest)

#include "tst_mediadevicetest.moc"

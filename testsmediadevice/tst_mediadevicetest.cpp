#include <QString>
#include <QtTest>
#include <QCoreApplication>

#include "streamingfile.h"
#include "ffmpegtranscoding.h"

class mediadeviceTest : public QObject
{
    Q_OBJECT

public:
    mediadeviceTest();

private Q_SLOTS:
    void testStreamingFile_data();
    void testStreamingFile();

    void testFfmpegTranscoding_data();
    void testFfmpegTranscoding();
};

mediadeviceTest::mediadeviceTest()
{
    FfmpegTranscoding::setDirPath("/opt/local/bin");
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

    // OPEN DEVICE
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

    // READ DEVICE UNTIL END
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
    QTest::newRow("MP3 RANGE=0-") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 159373 << 320000 << 6376920 << 800000 << 20 << 0 << -1 << 0 << -1 << -1 << -1 << -1 << -1;
    QTest::newRow("MP3 RANGE=100000-") << QString("/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Mister Mystère/1-02 Phébus.mp3") << 159373 << 159373 << 320000 << 6276920 << 800000 << 20 << 100000 << -1 << 100000 << -1 << -1 << -1 << -1 << -1;

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
    device.setLengthInMSeconds(param_durationInMSeconds);
    device.setFormat(MP3);
    device.setBitrate(bitrate);

    device.setRange(param_startByte, param_endByte);
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

    // OPEN DEVICE
    QCOMPARE(device.open(), true);
    device.waitForFinished(-1);

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

    // READ DEVICE UNTIL END
    qint64 readBytes = 0;
    while (!device.atEnd())
    {
        QByteArray data;
        data += device.read(1000);
        readBytes += data.size();

    }
    double delta_size = (double)(size-readBytes)/(double)size*100.0;
    qWarning() << "delta size" << delta_size;
    qWarning() << "bytes read" << readBytes << size;
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

QTEST_MAIN(mediadeviceTest)

#include "tst_mediadevicetest.moc"

#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QAudioFormat>
#include "qffmpeg.h"
#include "ffmpegtranscoding.h"
#include "spectrumanalyser.h"
#include "wavfile.h"

class TestsspectrumanalyserTest : public QObject
{
    Q_OBJECT

public:
    TestsspectrumanalyserTest();

public slots:
    void spectrumChanged(const FrequencySpectrum &spectrum);

private Q_SLOTS:
    void testWAV();
    void testMP3();

private:
    qreal m_freq;
};

TestsspectrumanalyserTest::TestsspectrumanalyserTest():
    m_freq(0.0)
{
}

void TestsspectrumanalyserTest::spectrumChanged(const FrequencySpectrum &spectrum)
{
    for (int i=0;i<spectrum.count();++i)
    {
        FrequencySpectrum::Element elt = spectrum[i];

        if (elt.frequency > 0.0 && elt.amplitude > 0.0)
        {
            if (elt.frequency > m_freq)
                m_freq = elt.frequency;
        }
    }
}

void TestsspectrumanalyserTest::testWAV()
{
    m_freq = 0.0;

    WavFile wav;
    QCOMPARE(wav.openLocalFile("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/test.wav"), true);
    QCOMPARE(wav.bytesPerSample(), 4);

    SpectrumAnalyser analyser;
    connect(&analyser, SIGNAL(spectrumChanged(FrequencySpectrum)), this, SLOT(spectrumChanged(FrequencySpectrum)));
    QCOMPARE(analyser.isReady(), true);

    QElapsedTimer timer;
    timer.start();

    qint64 bytesToRead = SpectrumLengthSamples*wav.bytesPerSample();
    while (wav.bytesAvailable() >= bytesToRead)
    {
        QByteArray buffer = wav.readSamples(SpectrumLengthSamples);
        if (buffer.size() == bytesToRead)
        {
            analyser.calculate(buffer, wav.fileFormat());
        }
        else
        {
            qWarning() << "not enough data";
        }
    }

    qInfo() << "bytes available" << wav.bytesAvailable();
    qInfo() << "FREQUENCY" << m_freq;
    QCOMPARE(qRound(m_freq), 22969);
    qInfo() << "analysis done in" << timer.elapsed() << "ms.";
}

void TestsspectrumanalyserTest::testMP3()
{
    m_freq = 0.0;

    FfmpegTranscoding::setDirPath("/opt/local/bin");

    QAudioFormat format;
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");
    format.setChannelCount(2);
    format.setSampleRate(48000);
    format.setSampleSize(16);
    format.setSampleType(format.sampleSize() == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);

    SpectrumAnalyser analyser;
    connect(&analyser, SIGNAL(spectrumChanged(FrequencySpectrum)), this, SLOT(spectrumChanged(FrequencySpectrum)));
    QCOMPARE(analyser.isReady(), true);

    FfmpegTranscoding decode_audio;
    decode_audio.setFormat(LPCM_S16LE);
    decode_audio.setOriginalLengthInMSeconds(120000);
    decode_audio.setAudioSampleRate(format.sampleRate());

    if (format.sampleRate() == 44100)
        decode_audio.setBitrate(1411000);
    else
        decode_audio.setBitrate(1536000);

    decode_audio.setUrl("/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/07 On_Off.mp3");

    QCOMPARE(decode_audio.lengthInMSeconds(), 120000);
    QCOMPARE(decode_audio.audioSampleRate(), 48000);
    QCOMPARE(decode_audio.bitrate(), 1536000);

    QElapsedTimer timer;
    timer.start();

    QCOMPARE(decode_audio.isReadyToOpen(), true);
    QVERIFY2(decode_audio.open() == true, "unable to start FFMPEG decoding");
    decode_audio.startRequestData();

    QVERIFY2(decode_audio.waitForFinished(-1) == true, "decoding not finished");

    while (decode_audio.bytesAvailable() > 0)
    {
        int bytesPerSample = format.sampleSize() * format.channelCount() / 8;

        QByteArray buffer = decode_audio.read(SpectrumLengthSamples*bytesPerSample);
        if (buffer.size() == SpectrumLengthSamples*bytesPerSample)
        {
            analyser.calculate(buffer, format);
        }
    }

    QCOMPARE(decode_audio.atEnd(), true);

    qInfo() << "bytes available" << decode_audio.bytesAvailable();
    qInfo() << "FREQUENCY" << m_freq;
    QCOMPARE(qRound(m_freq), 9691);
    qInfo() << "analysis done in" << timer.elapsed() << "ms.";
}

QTEST_MAIN(TestsspectrumanalyserTest)

#include "tst_testsspectrumanalysertest.moc"

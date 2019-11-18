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
    TestsspectrumanalyserTest() = default;

public slots:
    void spectrumChanged(const FrequencySpectrum &spectrum);

private Q_SLOTS:
    void cleanup();

    void test_data();
    void test();

private:
    qreal m_freq = 0.0;
};


void TestsspectrumanalyserTest::cleanup()
{
    DebugInfo::display_alive_objects();

    QCOMPARE(DebugInfo::count_alive_objects(), 0);
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

void TestsspectrumanalyserTest::test_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<int>("frequency");

    QTest::newRow("WAV") << "/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/test.wav" << 22969;
    QTest::newRow("MP3") << "/Users/doudou/workspaceQT/DLNA_server/tests/AUDIO/07 On_Off.mp3" << 9313;
}

void TestsspectrumanalyserTest::test()
{
    m_freq = 0.0;

    WavFile wav;
    QFETCH(QString, filename);
    QCOMPARE(wav.openLocalFile(filename), true);
    QCOMPARE(wav.bytesPerSample(), 4);
    qInfo() << wav.samplesAvailable() << "samples available.";
    qInfo() << "duration:" << wav.durationMsec() << "ms.";

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
    QFETCH(int, frequency);
    QCOMPARE(qRound(m_freq), frequency);
    qInfo() << "analysis done in" << timer.elapsed() << "ms.";
}

QTEST_MAIN(TestsspectrumanalyserTest)

#include "tst_testsspectrumanalysertest.moc"

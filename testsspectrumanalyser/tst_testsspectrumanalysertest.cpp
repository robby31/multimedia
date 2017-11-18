#include <QString>
#include <QtTest>
#include <QCoreApplication>

class TestsspectrumanalyserTest : public QObject
{
    Q_OBJECT

public:
    TestsspectrumanalyserTest();

private Q_SLOTS:
    void testMP3();
};

TestsspectrumanalyserTest::TestsspectrumanalyserTest()
{
}

void TestsspectrumanalyserTest::testMP3()
{
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(TestsspectrumanalyserTest)

#include "tst_testsspectrumanalysertest.moc"

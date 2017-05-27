#include <QString>
#include <QtTest>

class AcoustIdTest : public QObject
{
    Q_OBJECT

public:
    AcoustIdTest();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();
};

AcoustIdTest::AcoustIdTest()
{
}

void AcoustIdTest::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void AcoustIdTest::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(AcoustIdTest)

#include "tst_acoustidtest.moc"

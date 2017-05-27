#include <QString>
#include <QtTest>
#include <QCoreApplication>

class acoustidTest : public QObject
{
    Q_OBJECT

public:
    acoustidTest();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();
};

acoustidTest::acoustidTest()
{
}

void acoustidTest::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void acoustidTest::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_MAIN(acoustidTest)

#include "tst_acoustidtest.moc"

#include <QString>
#include <QtTest>

class ChromaprintWrapperTests : public QObject
{
    Q_OBJECT

public:
    ChromaprintWrapperTests();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();
};

ChromaprintWrapperTests::ChromaprintWrapperTests()
{
}

void ChromaprintWrapperTests::testCase1_data()
{
    QTest::addColumn<QString>("data");
    QTest::newRow("0") << QString();
}

void ChromaprintWrapperTests::testCase1()
{
    QFETCH(QString, data);
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(ChromaprintWrapperTests)

#include "tst_chromaprintwrappertests.moc"

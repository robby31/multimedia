#ifndef ACOUSTIDCLIENT_H
#define ACOUSTIDCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QtXml>

#include "acoustidanswer.h"

class AcoustIdClient : public QObject
{
    Q_OBJECT

public:
    explicit AcoustIdClient(QObject *parent = 0);

    AcoustIdAnswer *waitReply(const int &timeout=2000);

signals:
    void tagFound(AcoustIdAnswer *answer);

public slots:
    void requestId(const QString &fingerprint, const int &duration);
    void replyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager manager;
    QMutex mutex;
    QWaitCondition replyWaitCondition;

    QString key;

    AcoustIdAnswer *last_answer;

    static const QString ACOUSTID_URL;
};

#endif // ACOUSTIDCLIENT_H

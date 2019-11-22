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
    explicit AcoustIdClient(QObject *parent = Q_NULLPTR);

    void requestId(const QString &fingerprint, const int &duration);

    AcoustIdAnswer *waitReply(const int &timeout = 5000);

private slots:
    void replyFinished();

private:
    QNetworkAccessManager manager;
    QString key;
    QEventLoop loop;
    AcoustIdAnswer *last_answer = Q_NULLPTR;

    static const QUrl ACOUSTID_URL;
};

#endif // ACOUSTIDCLIENT_H

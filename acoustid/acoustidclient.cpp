#include "acoustidclient.h"

const QString AcoustIdClient::ACOUSTID_URL = "http://api.acoustid.org/v2/lookup";

AcoustIdClient::AcoustIdClient(QObject *parent):
    QObject(parent),
    manager(this),
    key("8XaBELgH"),
    last_answer(0)
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

void AcoustIdClient::replyFinished(QNetworkReply *reply)
{
    last_answer = new AcoustIdAnswer();
    last_answer->setContent(reply->readAll());
    emit tagFound(last_answer);
    replyWaitCondition.wakeAll();
}

void AcoustIdClient::requestId(const QString &fingerprint, const int &duration)
{
    QStringList parameters;

    parameters << QString("%1=%2").arg("format").arg("xml");
    parameters << QString("%1=%2").arg("client").arg(key);

    parameters << QString("%1=%2").arg("duration").arg(QString("%1").arg(duration));

    parameters << QString("%1=%2").arg("fingerprint").arg(fingerprint);

    QStringList meta;
    meta << "recordings";
//    meta << "recordingids";
//    meta << "releases";
//    meta << "releaseids";
    meta << "releasegroups";
//    meta << "releasegroupids";
    meta << "tracks";
//    meta << "usermeta";
    meta << "compress";
//    meta << "sources";
    parameters << QString("%1=%2").arg("meta").arg(meta.join('+'));

    manager.get(QNetworkRequest(QUrl(ACOUSTID_URL+"?"+parameters.join('&'))));
}

AcoustIdAnswer *AcoustIdClient::waitReply(const int &timeout)
{
    // waiting reply with timeout
    mutex.lock();
    bool ret = replyWaitCondition.wait(&mutex, timeout);
    mutex.unlock();
    if (ret)
        return last_answer;
    else
        return 0;
}

#include "acoustidclient.h"

const QString AcoustIdClient::ACOUSTID_URL = "http://api.acoustid.org/v2/lookup";

AcoustIdClient::AcoustIdClient(QObject *parent):
    QObject(parent),
    manager(this),
    key("29E2YuWCkD")
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

    parameters << QString("%1=%2").arg("format", "xml");
    parameters << QString("%1=%2").arg("client", key);

    parameters << QString("%1=%2").arg("duration", QString("%1").arg(duration));

    parameters << QString("%1=%2").arg("fingerprint", fingerprint);

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
    parameters << QString("%1=%2").arg("meta", meta.join('+'));

    qDebug() << "request URL" << QUrl(ACOUSTID_URL+"?"+parameters.join('&'));
    manager.get(QNetworkRequest(QUrl(ACOUSTID_URL+"?"+parameters.join('&'))));
}

AcoustIdAnswer *AcoustIdClient::waitReply(const size_t &timeout)
{
    // waiting reply with timeout
    mutex.lock();
    bool ret = replyWaitCondition.wait(&mutex, timeout);
    mutex.unlock();
    if (ret)
        return last_answer;

    return Q_NULLPTR;
}

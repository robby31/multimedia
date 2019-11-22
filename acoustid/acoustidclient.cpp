#include "acoustidclient.h"

const QUrl AcoustIdClient::ACOUSTID_URL = QUrl("http://api.acoustid.org/v2/lookup");

AcoustIdClient::AcoustIdClient(QObject *parent):
    QObject(parent),
    manager(this),
    key("29E2YuWCkD"),
    loop(this)
{
}

void AcoustIdClient::replyFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
    {
        qCritical() << "invalid reply";
        return;
    }

    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        qCritical() << "reply ERROR" << reply->errorString();
        return;
    }

    last_answer = new AcoustIdAnswer(this);
    last_answer->setContent(reply->readAll());

    qDebug() << "TAG FOUND" << last_answer;
    qDebug() << last_answer->content();
}

void AcoustIdClient::requestId(const QString &fingerprint, const int &duration)
{
    qDebug() << "request id to acoustIdClient" << fingerprint.size() << duration;

    last_answer = Q_NULLPTR;

    QUrlQuery query;

    query.addQueryItem("format", "xml");
    query.addQueryItem("client", key);

    query.addQueryItem("duration", QString("%1").arg(duration));

    query.addQueryItem("fingerprint", fingerprint);

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
    query.addQueryItem("meta", meta.join('+'));

    QUrl url(ACOUSTID_URL);
    url.setQuery(query);
    qDebug() << "request URL" << url;

    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &AcoustIdClient::replyFinished);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
}

AcoustIdAnswer *AcoustIdClient::waitReply(const int &timeout)
{
    if (last_answer)
        return last_answer;

    QTimer timer;
    if (timeout > 0)
    {
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start(timeout);
    }

    loop.exec();

    if (!last_answer)
        qWarning() << "TIMEOUT";

    return last_answer;
}

#include "acoustid.h"

Acoustid::Acoustid(QObject *parent):
    QObject(parent),
    client(this)
{
}

AcoustIdAnswer *Acoustid::requestId(const QString &fingerprint, const int &duration, const int &timeout)
{
    client.requestId(fingerprint, duration);

    AcoustIdAnswer *answer = client.waitReply(timeout);
    return answer;
}

AcoustIdAnswer *Acoustid::requestId(const QFileInfo &filename, const int &timeout)
{
    int duration = -1;

    ChromaprintWrapper chromaprint;
    QString fp = chromaprint.fingerPrint(filename.absoluteFilePath(), &duration);

    qDebug() << "fingerprint, size =" << fp.size();

    if (!fp.isNull())
        return requestId(fp, duration, timeout);

    return Q_NULLPTR;
}

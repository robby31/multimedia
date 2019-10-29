#include "acoustid.h"

Acoustid::Acoustid(QObject *parent):
    QObject(parent),
    workerThread(this)
{
    client = new AcoustIdClient();
    client->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, client, &QObject::deleteLater);
    connect(this, SIGNAL(requestClient(QString,int)), client, SLOT(requestId(QString,int)));
    workerThread.start();
}

Acoustid::~Acoustid()
{
    workerThread.quit();
    if (!workerThread.wait(1000))
        qWarning() << "Unable to stop acoustid thread.";
}

AcoustIdAnswer *Acoustid::requestId(const QString &fingerprint, const int &duration)
{
    emit requestClient(fingerprint, duration);
    if (client) {
        AcoustIdAnswer *answer = client->waitReply();
        return answer;
    }
    return Q_NULLPTR;
}

AcoustIdAnswer *Acoustid::requestId(const QFileInfo &filename)
{
    int duration = -1;
    QString fp = m_chromaprint.fingerPrint(filename.absoluteFilePath(), &duration);
    if (!fp.isNull())
        return requestId(fp, duration);
    return Q_NULLPTR;
}

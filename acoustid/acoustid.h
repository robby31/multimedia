#ifndef ACOUSTID_H
#define ACOUSTID_H

#include <QObject>

#include "acoustidclient.h"
#include "acoustidanswer.h"
#include "chromaprint_wrapper.h"

class Acoustid : public QObject
{
    Q_OBJECT

public:
    explicit Acoustid(QObject *parent = 0);
    virtual ~Acoustid();

    AcoustIdAnswer *requestId(const QString &fingerprint, const int &duration);
    AcoustIdAnswer *requestId(const QFileInfo &filename);

signals:
    void requestClient(const QString &fingerprint, const int &duration) const;

private slots:

private:
    QThread workerThread;
    AcoustIdClient *client;
    ChromaprintWrapper m_chromaprint;
};

#endif // ACOUSTID_H

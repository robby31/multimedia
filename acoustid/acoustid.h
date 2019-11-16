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
    explicit Acoustid(QObject *parent = Q_NULLPTR);
    ~Acoustid() Q_DECL_OVERRIDE;
    Acoustid(Acoustid const&) = delete;
    Acoustid& operator =(Acoustid const&) = delete;
    Acoustid(Acoustid&&) = delete;
    Acoustid& operator=(Acoustid&&) = delete;

    AcoustIdAnswer *requestId(const QString &fingerprint, const int &duration);
    AcoustIdAnswer *requestId(const QFileInfo &filename);

signals:
    void requestClient(const QString &fingerprint, const int &duration) const;

private slots:

private:
    QThread workerThread;
    AcoustIdClient *client = Q_NULLPTR;
    ChromaprintWrapper m_chromaprint;
};

#endif // ACOUSTID_H

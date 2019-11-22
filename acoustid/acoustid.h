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

    AcoustIdAnswer *requestId(const QString &fingerprint, const int &duration, const int &timeout = 5000);
    AcoustIdAnswer *requestId(const QFileInfo &filename, const int &timeout = 5000);

private:
    AcoustIdClient client;
};

#endif // ACOUSTID_H

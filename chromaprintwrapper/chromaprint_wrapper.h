#ifndef CHROMAPRINT_WRAPPER_H
#define CHROMAPRINT_WRAPPER_H

#include <QDebug>
#include <QProcess>
#include <QString>
#include <QtXml>

#include <chromaprint.h>

class ChromaprintWrapper
{

public:
    ChromaprintWrapper();
    ~ChromaprintWrapper();
    ChromaprintWrapper(ChromaprintWrapper const&) = delete;
    ChromaprintWrapper& operator =(ChromaprintWrapper const&) = delete;
    ChromaprintWrapper(ChromaprintWrapper&&) = delete;
    ChromaprintWrapper& operator=(ChromaprintWrapper&&) = delete;

    QString get_version() const;

    QString fingerPrint(const QString &file_name, int *duration, const int &max_length=120);

private:
    int decode_audio_file(ChromaprintContext *chromaprint_ctx, const QString &file_name, const int &max_length, int *p_duration);

private:
    int algo = CHROMAPRINT_ALGORITHM_DEFAULT;
    ChromaprintContext *chromaprint_ctx;
    QProcess programFfmpeg;
};

#endif // CHROMAPRINT_WRAPPER_H

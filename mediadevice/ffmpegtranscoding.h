#ifndef FFMPEGTRANSCODING_H
#define FFMPEGTRANSCODING_H

#include <QDir>
#include "transcodeprocess.h"

class FfmpegTranscoding : public TranscodeProcess
{
    Q_OBJECT

public:
    explicit FfmpegTranscoding(QObject *parent = Q_NULLPTR);

    static void setDirPath(const QString &folder);

    void setVariableBitrate(const bool &flag);

private:
    virtual void updateArguments() Q_DECL_OVERRIDE;

signals:

public slots:

private:
    static QString EXE_DIRPATH;
    bool variable_bitrate;
};

#endif // FFMPEGTRANSCODING_H

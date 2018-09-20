#ifndef MENCODERTRANSCODING_H
#define MENCODERTRANSCODING_H

#include "transcodeprocess.h"

class MencoderTranscoding : public TranscodeProcess
{
    Q_OBJECT

public:
    explicit MencoderTranscoding(QObject *parent = Q_NULLPTR);

private:
    void updateArguments() Q_DECL_OVERRIDE;

signals:

public slots:

private:
    static const QString PROGRAM;
};

#endif // MENCODERTRANSCODING_H

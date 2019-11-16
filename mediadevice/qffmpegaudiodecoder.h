#ifndef QFFMPEGAUDIODECODER_H
#define QFFMPEGAUDIODECODER_H

#include "qffmpegdecoder.h"

class QFfmpegAudioDecoder : public QFfmpegDecoder
{
    Q_OBJECT

public:
    explicit QFfmpegAudioDecoder(QObject *parent = Q_NULLPTR);
};

#endif // QFFMPEGAUDIODECODER_H

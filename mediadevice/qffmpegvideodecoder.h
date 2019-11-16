#ifndef QFFMPEGVIDEODECODER_H
#define QFFMPEGVIDEODECODER_H

#include "qffmpegdecoder.h"

class QFfmpegVideoDecoder: public QFfmpegDecoder
{
    Q_OBJECT

public:
    explicit QFfmpegVideoDecoder(QObject *parent = Q_NULLPTR);
};

#endif // QFFMPEGVIDEODECODER_H

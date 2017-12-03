#ifndef QFFMPEGPACKET_H
#define QFFMPEGPACKET_H

#include "qglobal.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class QFfmpegPacket
{
public:
    QFfmpegPacket();
    virtual ~QFfmpegPacket();

    bool isValid() const;
    AVPacket *ptr() const;

public:
    static qint64 objectCounter;

private:
    AVPacket *m_pkt = NULL;
};

#endif // QFFMPEGPACKET_H

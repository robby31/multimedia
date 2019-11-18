#ifndef QFFMPEGPACKET_H
#define QFFMPEGPACKET_H

#include "qglobal.h"
#include "debuginfo.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

class QFfmpegPacket : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY_MOVE(QFfmpegPacket)

public:
    explicit QFfmpegPacket(QObject *parent = Q_NULLPTR);
    ~QFfmpegPacket() Q_DECL_OVERRIDE;

    bool isValid() const;
    AVPacket *ptr() const;

private:
    AVPacket *m_pkt = Q_NULLPTR;
};

#endif // QFFMPEGPACKET_H

#include "qffmpegpacket.h"

qint64 QFfmpegPacket::objectCounter = 0;

QFfmpegPacket::QFfmpegPacket():
    m_pkt(av_packet_alloc())
{
    objectCounter++;
}

QFfmpegPacket::~QFfmpegPacket()
{
    objectCounter--;

    if (isValid())
        av_packet_free(&m_pkt);
}

bool QFfmpegPacket::isValid() const
{
    return m_pkt != Q_NULLPTR;
}

AVPacket *QFfmpegPacket::ptr() const
{
    return m_pkt;
}

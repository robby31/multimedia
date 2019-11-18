#include "qffmpegpacket.h"

QFfmpegPacket::QFfmpegPacket(QObject *parent):
    QObject(parent),
    m_pkt(av_packet_alloc())
{
    DebugInfo::add_object(this);
}

QFfmpegPacket::~QFfmpegPacket()
{
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

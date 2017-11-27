#include "qffmpegbuffer.h"

qint64 QFfmpegBuffer::objectCounter = 0;

QFfmpegBuffer::QFfmpegBuffer()
{
    objectCounter++;

    avio_ctx_buffer = (uint8_t*)av_malloc(avio_ctx_buffer_size);
    if (avio_ctx_buffer != NULL)
    {
        avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size,
                                      1, &buffer, &QFfmpegBuffer::read_packet, &QFfmpegBuffer::write_packet, NULL);
    }
}

QFfmpegBuffer::~QFfmpegBuffer()
{
    objectCounter--;

    if (avio_ctx != NULL)
    {
        av_freep(&avio_ctx->buffer);
        av_freep(&avio_ctx);
    }
}

bool QFfmpegBuffer::isValid() const
{
    return avio_ctx_buffer != NULL && avio_ctx != NULL;
}

AVIOContext *QFfmpegBuffer::context() const
{
    return avio_ctx;
}

bool QFfmpegBuffer::atEnd() const
{
    return bytesAvailable() == 0;
}

qint64 QFfmpegBuffer::bytesAvailable() const
{
    return buffer.size();
}

QByteArray QFfmpegBuffer::read(const int &maxlen)
{
    qint64 bytesToRead = qMin(buffer.size(), maxlen);
    QByteArray res = buffer.left(bytesToRead);
    buffer.remove(0, res.size());
    return res;
}

int QFfmpegBuffer::read_packet(void *opaque, uint8_t *buf, int buf_size)
{
    QByteArray *bd = (QByteArray *)opaque;

    /* copy internal buffer data to buf */
    memcpy(buf, bd->data(), buf_size);
    bd->remove(0, buf_size);

    return buf_size;
}

int QFfmpegBuffer::write_packet(void *opaque, uint8_t *buf, int buf_size)
{
    QByteArray *bd = (QByteArray *)opaque;

    bd->append(QByteArray::fromRawData((char *)buf, buf_size));

    return buf_size;
}

void QFfmpegBuffer::flush()
{
    if (avio_ctx != NULL)
        avio_flush(avio_ctx);
}

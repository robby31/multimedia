#include "qffmpegbuffer.h"

qint64 QFfmpegBuffer::objectCounter = 0;

QFfmpegBuffer::QFfmpegBuffer()
{
    objectCounter++;

    auto avio_ctx_buffer = (uint8_t*)av_malloc(avio_ctx_buffer_size);
    if (avio_ctx_buffer != Q_NULLPTR)
    {
        avio_ctx = avio_alloc_context(avio_ctx_buffer, avio_ctx_buffer_size, 1, this,
                                      &QFfmpegBuffer::read_from_context, &QFfmpegBuffer::write_from_context, &QFfmpegBuffer::seek_from_context);
    }
}

QFfmpegBuffer::~QFfmpegBuffer()
{
    objectCounter--;

    if (avio_ctx != Q_NULLPTR)
    {
        av_freep(&avio_ctx->buffer);
        av_freep(&avio_ctx);
    }
}

bool QFfmpegBuffer::isValid() const
{
    return avio_ctx != Q_NULLPTR && avio_ctx->buffer != Q_NULLPTR;
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
    return m_buffer.size();
}

QByteArray QFfmpegBuffer::read(const qint64 &maxlen)
{
    qint64 bytesToRead = qMin((qint64)m_buffer.size(), maxlen);
    QByteArray res = m_buffer.left(bytesToRead);
    m_buffer.remove(0, res.size());

    m_startPos += res.size();
    m_pos += res.size();

    return res;
}

int QFfmpegBuffer::read_from_context(void *opaque, uint8_t *buf, int buf_size)
{
    auto buffer = static_cast<QFfmpegBuffer *>(opaque);
    QByteArray *bd = &buffer->m_buffer;

    if (buffer && bd)
    {
        /* copy internal buffer data to buf */
        memcpy(buf, bd->data(), buf_size);
        bd->remove(0, buf_size);
        return buf_size;
    }

    return 0;
}

int QFfmpegBuffer::write_from_context(void *opaque, uint8_t *buf, int buf_size)
{
    auto buffer = static_cast<QFfmpegBuffer *>(opaque);

    if (buffer)
    {
        if (buffer->write(QByteArray::fromRawData((char *)buf, buf_size)))
            return buf_size;
    }

    return 0;
}

int64_t QFfmpegBuffer::seek_from_context(void *opaque, int64_t offset, int whence)
{
    auto buffer = static_cast<QFfmpegBuffer *>(opaque);

    if (buffer)
    {
        if (buffer->seek(whence + offset))
            return whence + offset;
    }

    qCritical() << "unable to seek" << offset << whence;
    return -1;
}

bool QFfmpegBuffer::write(const QByteArray &data)
{
    if (m_pos >= m_startPos && m_pos <= m_endPos)
    {
        // position is valid
        m_buffer.append(data);
        m_endPos += data.size();
        return true;
    }

    // position is out of buffer data
    qWarning() << "unable to write" << data.size() << "bytes data because position" << m_pos << "is invalid, start pos =" << m_startPos << "end pos = " << m_endPos;
    return false;
}

bool QFfmpegBuffer::seek(const qint64 &pos)
{
    m_pos = pos;
    return true;
}

void QFfmpegBuffer::flush()
{
    if (avio_ctx != Q_NULLPTR)
        avio_flush(avio_ctx);
}

#ifndef QFFMPEGBUFFER_H
#define QFFMPEGBUFFER_H

#include <QDebug>
#include "debuginfo.h"

extern "C" {
#include <libavformat/avio.h>
}

class QFfmpegBuffer : public QObject
{
    Q_OBJECT

public:
    explicit QFfmpegBuffer(QObject *parent = Q_NULLPTR);
    ~QFfmpegBuffer() Q_DECL_OVERRIDE;
    QFfmpegBuffer(QFfmpegBuffer const&) = delete;
    QFfmpegBuffer& operator =(QFfmpegBuffer const&) = delete;
    QFfmpegBuffer(QFfmpegBuffer&&) = delete;
    QFfmpegBuffer& operator=(QFfmpegBuffer&&) = delete;

    bool isValid() const;

    AVIOContext *context() const;

    bool atEnd() const;
    qint64 bytesAvailable() const;
    QByteArray read(const qint64 &maxlen);
    void flush();

private:
    static int read_from_context(void *opaque, uint8_t *buf, int buf_size);
    static int write_from_context(void *opaque, uint8_t *buf, int buf_size);
    static int64_t seek_from_context(void *opaque, int64_t offset, int whence);

    bool write(const QByteArray &data);
    bool seek(const qint64 &pos);

private:
    AVIOContext *avio_ctx = Q_NULLPTR;
    size_t avio_ctx_buffer_size = 4096;

    QByteArray m_buffer;
    qint64 m_pos = 0;
    qint64 m_startPos = 0;
    qint64 m_endPos = 0;
};

#endif // QFFMPEGBUFFER_H

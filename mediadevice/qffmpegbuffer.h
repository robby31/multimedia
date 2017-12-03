#ifndef QFFMPEGBUFFER_H
#define QFFMPEGBUFFER_H

#include <QDebug>

extern "C" {
#include <libavformat/avio.h>
}

class QFfmpegBuffer
{

public:
    QFfmpegBuffer();
    virtual ~QFfmpegBuffer();

    bool isValid() const;

    AVIOContext *context() const;

    bool atEnd() const;
    qint64 bytesAvailable() const;
    QByteArray read(const int &maxlen);
    void flush();

private:
    static int read_from_context(void *opaque, uint8_t *buf, int buf_size);
    static int write_from_context(void *opaque, uint8_t *buf, int buf_size);
    static int64_t seek_from_context(void *opaque, int64_t offset, int whence);

    bool write(const QByteArray &data);
    bool seek(const qint64 &pos);

public:
    static qint64 objectCounter;

private:
    AVIOContext *avio_ctx = NULL;
    size_t avio_ctx_buffer_size = 4096;

    QByteArray m_buffer;
    qint64 m_pos = 0;
    qint64 m_startPos = 0;
    qint64 m_endPos = 0;
};

#endif // QFFMPEGBUFFER_H

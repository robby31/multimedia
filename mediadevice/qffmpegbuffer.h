#ifndef QFFMPEGBUFFER_H
#define QFFMPEGBUFFER_H

#include <QDebug>

extern "C" {
#include <libavformat/avio.h>
}

class QFfmpegBuffer
{

    struct buffer_data {
        uint8_t *ptr;
        int size; ///< size left in the buffer
    };

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
    static int read_packet(void *opaque, uint8_t *buf, int buf_size);
    static int write_packet(void *opaque, uint8_t *buf, int buf_size);

public:
    static qint64 objectCounter;

private:
    AVIOContext *avio_ctx = NULL;
    uint8_t *avio_ctx_buffer = NULL;
    size_t avio_ctx_buffer_size = 4096;
    QByteArray buffer;
};

#endif // QFFMPEGBUFFER_H

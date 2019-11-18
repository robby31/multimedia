/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qendian.h>
#include <QVector>
#include <QDebug>
#include "wavfile.h"

struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
};

WavFile::WavFile(QObject *parent):
    QObject(parent)
{
    DebugInfo::add_object(this);
}

bool WavFile::openLocalFile(const QString &fileName)
{
    if (m_device)
    {
        m_device->close();
        m_device->deleteLater();
        m_device = Q_NULLPTR;
    }

    m_device = new QFile(fileName, this);
    DebugInfo::add_object(m_device);

    if (!m_device->open(QIODevice::ReadOnly))
    {
        m_error = InvalidFile;
        m_device->deleteLater();
        m_device = Q_NULLPTR;
        return false;
    }

    if (!readHeader())
    {
        m_device->close();
        m_device->deleteLater();
        m_device = Q_NULLPTR;

        // file is not a WAV format so we transcode it in wave

        #if !defined(QT_NO_DEBUG_OUTPUT)
        qDebug() << fileName << "is not a WAV file, so we transcode it.";
        #endif

        QFfmpegTranscoding decode_audio;
        decode_audio.setFormat(WAV);
        decode_audio.setUrl(fileName);

        if (decode_audio.isReadyToOpen() && decode_audio.open())
        {
            m_device = new QBuffer(this);
            DebugInfo::add_object(m_device);

            if (!m_device->open(QIODevice::ReadWrite))
            {
                qCritical() << "unable to open buffer";
                m_error = InvalidFile;
                m_device->deleteLater();
                m_device = Q_NULLPTR;
                return false;
            }

            while (!decode_audio.atEnd())
                m_device->write(decode_audio.read(1000000));

            if (!readHeader())
            {
                qCritical() << "invalid header";
                m_error = InvalidHeader;
                m_device->close();
                m_device->deleteLater();
                m_device = Q_NULLPTR;
                return false;
            }

            return true;
        }

        m_error = InvalidFile;
        return false;
    }

    return true;
}

const QAudioFormat &WavFile::fileFormat() const
{
    return m_fileFormat;
}

qint64 WavFile::headerLength() const
{
    return m_headerLength;
}

bool WavFile::readHeader()
{
    bool result = false;

    if (m_device)
    {
        m_device->seek(0);
        CombinedHeader header;
        result = m_device->read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader);
        if (result) {
            if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
                 || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
                    && memcmp(&header.riff.type, "WAVE", 4) == 0
                    && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
                    && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)) {

                // Read off remaining header information

                if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
                    // Extended data available
                    quint16 extraFormatBytes;
                    if (m_device->peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                        return false;
                    const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
                    if (m_device->read(throwAwayBytes).size() != throwAwayBytes)
                        return false;
                }

                DATAHeader dataHeader;
                if (m_device->read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
                    return false;

                // Establish format
                if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                    m_fileFormat.setByteOrder(QAudioFormat::LittleEndian);
                else
                    m_fileFormat.setByteOrder(QAudioFormat::BigEndian);

                int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
                m_fileFormat.setChannelCount(qFromLittleEndian<quint16>(header.wave.numChannels));
                m_fileFormat.setCodec("audio/pcm");
                m_fileFormat.setSampleRate(static_cast<int>(qFromLittleEndian<quint32>(header.wave.sampleRate)));
                m_fileFormat.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
                m_fileFormat.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
            } else {
                result = false;
            }
        }
        m_headerLength = m_device->pos();
    }
    return result;
}

qint64 WavFile::bytesPerSample() const
{
    return m_fileFormat.channelCount() * m_fileFormat.sampleSize() / 8;
}

qint64 WavFile::bitrate() const
{
    return bytesPerSample() * m_fileFormat.sampleRate() * 8;
}

qint64 WavFile::bytesAvailable() const
{
    if (m_device)
        return m_device->bytesAvailable();

    return -1;
}

qint64 WavFile::samplesAvailable() const
{
    if (bytesPerSample() != 0)
        return bytesAvailable() / bytesPerSample();

    return -1;
}

QByteArray WavFile::readSamples(const qint64 &nbSamples)
{
    if (m_device)
        return m_device->read(nbSamples*bytesPerSample());

    return QByteArray();
}

QByteArray WavFile::readSamplesAtPosMsec(const qint64 &posMsec, const qint64 &nbSamples)
{
    seek(m_headerLength + audioLength(posMsec));
    return readSamples(nbSamples);
}

qint64 WavFile::audioLength(const qint64 &posMsec) const
{
    qint64 posByte = (m_fileFormat.sampleRate() * bytesPerSample()) * posMsec / 1000;
    posByte -= posByte % (m_fileFormat.channelCount() * m_fileFormat.sampleSize());
    return posByte;
}

qint64 WavFile::audioDurationMsec(qint64 bytes) const
{
    return (bytes * 1000) / (m_fileFormat.sampleRate() * bytesPerSample());
}

bool WavFile::seek(const qint64 &posMsec)
{
    if (m_device)
        return m_device->seek(m_headerLength + posMsec);

    return false;
}

qint64 WavFile::pos() const
{
    if (m_device)
        return m_device->pos();

    return -1;
}

qint64 WavFile::size() const
{
    if (m_device)
        return m_device->size();

    return -1;
}

qint64 WavFile::durationMsec() const
{
    return audioDurationMsec(size() - m_headerLength);
}

WavFile::TypeError WavFile::error() const
{
    return m_error;
}

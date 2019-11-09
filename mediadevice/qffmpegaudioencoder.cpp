#include "qffmpegaudioencoder.h"

QFfmpegAudioEncoder::~QFfmpegAudioEncoder()
{
    _close();
}

void QFfmpegAudioEncoder::close()
{
    _close();
}

void QFfmpegAudioEncoder::_close()
{
    if (m_resampleCtx != Q_NULLPTR)
        swr_free(&m_resampleCtx);

    if (m_audioFifo != Q_NULLPTR)
    {
        if (av_audio_fifo_size(m_audioFifo) > 0)
            qWarning() << av_audio_fifo_size(m_audioFifo) << "samples remains in audio fifo of encoder" << format();

        av_audio_fifo_free(m_audioFifo);
        m_audioFifo = Q_NULLPTR;
    }

    QFfmpegEncoder::close();
}

bool QFfmpegAudioEncoder::setInput(QFfmpegCodec *codec_input)
{
    if (codec_input)
    {
        if (samplerate() == 0)
            setSampleRate(codec_input->samplerate());

        if (channelLayout() == 0)
        {
            if (codec_input->channelLayout() != 0)
                setChannelLayout(codec_input->channelLayout());
            else
                setChannelCount(codec_input->channelCount());
        }

        // initialize resampler for audio
        if (!init_resampler(codec_input))
        {
            qCritical() << "unable to initialise audio resampling.";
            close();
            return false;
        }

        return init_audio_fifo();
    }

    qCritical() << "invalid input codec" << codec_input;
    return false;
}

bool QFfmpegAudioEncoder::init_audio_fifo()
{
    /* Create the FIFO buffer based on the specified output sample format. */
    m_audioFifo = av_audio_fifo_alloc(sampleFormat(), channelCount(), 1);
    if (m_audioFifo == Q_NULLPTR)
    {
        qCritical() << "Could not allocate FIFO";
        close();
        return false;
    }

    return true;
}
bool QFfmpegAudioEncoder::init_resampler(QFfmpegCodec *input)
{
    if (input)
    {
        AVCodecContext *inputCodecCtx = input->codecCtx();

        if (inputCodecCtx != Q_NULLPTR)
        {
            if (inputCodecCtx->sample_rate != samplerate())
            {
                qWarning() << "sample rate for output shall not be different than input, sample rate will be set at" << inputCodecCtx->sample_rate;
                setSampleRate(inputCodecCtx->sample_rate);
            }

            /* create resampler context */
            m_resampleCtx = swr_alloc_set_opts(Q_NULLPTR,
                                               av_get_default_channel_layout(channelCount()),
                                               sampleFormat(),
                                               samplerate(),
                                               av_get_default_channel_layout(inputCodecCtx->channels),
                                               inputCodecCtx->sample_fmt,
                                               inputCodecCtx->sample_rate,
                                               0, Q_NULLPTR);
            if (m_resampleCtx == Q_NULLPTR)
            {
                qCritical() << "Could not allocate resampler context";
                close();
                return false;
            }

            /* initialize the resampling context */
            if (swr_init(m_resampleCtx) < 0)
            {
                qCritical() << "Failed to initialize the resampling context";
                close();
                return false;
            }

            #if !defined(QT_NO_DEBUG_OUTPUT)
            qDebug() << "resampling initialised:";
            qDebug() << "   channel count  :" << inputCodecCtx->channels << "-->" << channelCount();
            qDebug() << "   sample rate    :" << inputCodecCtx->sample_rate << "-->" << samplerate();
            qDebug() << "   sample format  :" << av_get_sample_fmt_name(inputCodecCtx->sample_fmt) << "-->" << av_get_sample_fmt_name(sampleFormat());
            #endif

            return true;
        }

        qCritical() << "invalid input codec context in resampler initialisation :" << inputCodecCtx;
        return false;
    }

    qCritical() << "invalid input in resampler initialisation :" << input;
    return false;
}

QFfmpegFrame *QFfmpegAudioEncoder::resampleFrame(QFfmpegFrame *frame)
{
    if (m_resampleCtx != Q_NULLPTR && frame != Q_NULLPTR)
    {
        auto newFrame = new QFfmpegFrame();
        if (!newFrame->isValid())
        {
            qCritical() << "Error allocation new frame.";
            delete newFrame;
            return Q_NULLPTR;
        }

        if (!newFrame->init_frame(sampleFormat(), channelLayout(), samplerate(), swr_get_out_samples(m_resampleCtx, frame->ptr()->nb_samples)))
        {
            delete newFrame;
            return Q_NULLPTR;
        }

        if (av_frame_make_writable(newFrame->ptr()) < 0)
        {
            qCritical() << "unable to create new frame for resampling." << newFrame->isValid();
            delete newFrame;
            return Q_NULLPTR;
        }

        /* convert to destination format */
        if (swr_convert(m_resampleCtx,
                        newFrame->ptr()->data, newFrame->ptr()->nb_samples,
                        (const uint8_t **)frame->ptr()->data, frame->ptr()->nb_samples) < 0)
        {
            qCritical() << "Error while converting";
            delete newFrame;
            return Q_NULLPTR;
        }

        return newFrame;
    }

    qCritical() << "resample context is not initialised.";
    return Q_NULLPTR;
}

bool QFfmpegAudioEncoder::encodeFrameFromFifo(const int &frame_size)
{
    auto newFrame = new QFfmpegFrame();
    if (!newFrame->isValid() || !newFrame->init_frame(sampleFormat(), channelLayout(), samplerate(), swr_get_out_samples(m_resampleCtx, frame_size)))
    {
        qCritical() << "Error allocation new frame.";
        delete newFrame;
        return false;
    }

    // read data from audio fifo
    if (av_audio_fifo_read(m_audioFifo, (void **)newFrame->ptr()->data, frame_size) < frame_size)
    {
        qCritical() << "Could not read data from FIFO";
        delete newFrame;
        return false;
    }

    AVRational bq;
    bq.num = 1;
    bq.den = samplerate();
    newFrame->ptr()->pts = av_rescale_q(nextPts(), bq, codecCtx()->time_base);
    incrNextPts(newFrame->ptr()->nb_samples);

    if (!QFfmpegEncoder::encodeFrame(newFrame))
    {
        delete newFrame;
        return false;
    }

    delete newFrame;
    return true;
}

bool QFfmpegAudioEncoder::encodeFrame(QFfmpegFrame *frame)
{
    bool result = false;

    if (isValid())
    {
        if (frame == Q_NULLPTR)
        {
            // flush encoder
            if (av_audio_fifo_size(m_audioFifo) > 0 && !encodeFrameFromFifo(av_audio_fifo_size(m_audioFifo)))
                return false;

            return QFfmpegEncoder::encodeFrame(Q_NULLPTR);
        }

        int frameSize = codecCtx()->frame_size;

        QFfmpegFrame *inputFrame = resampleFrame(frame);

        if (inputFrame != Q_NULLPTR)
        {
            add_samples_to_fifo(inputFrame);

            if (frameSize <= 0)
                frameSize = inputFrame->ptr()->nb_samples;

            delete inputFrame;
        }

        while (av_audio_fifo_size(m_audioFifo) >= frameSize)
        {
            if (!encodeFrameFromFifo(frameSize))
            {
                result = false;
                break;
            }

            result = true;
        }
    }
    else
    {
        qCritical() << "codec is invalid for encoding.";
    }

    return result;
}

bool QFfmpegAudioEncoder::add_samples_to_fifo(QFfmpegFrame *frame)
{
    if (frame)
    {
        /* Make the FIFO as large as it needs to be to hold both,
         * the old and the new samples. */
        if (av_audio_fifo_realloc(m_audioFifo, av_audio_fifo_size(m_audioFifo) + frame->ptr()->nb_samples) < 0)
        {
            qCritical() << "Could not reallocate FIFO";
            return false;
        }

        /* Store the new samples in the FIFO buffer. */
        if (av_audio_fifo_write(m_audioFifo, (void**)&frame->ptr()->data, frame->ptr()->nb_samples) < frame->ptr()->nb_samples)
        {
            qCritical() << "Could not write data to FIFO";
            return false;
        }

        return true;
    }

    qCritical() << "invalid frame" << frame << "cannot add samples to audio fifo.";
    return false;
}

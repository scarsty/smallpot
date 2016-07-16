#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
}

#define BP_AUDIO_RESAMPLE_FORMAT AV_SAMPLE_FMT_S16

class BigPotResample
{
public:
    BigPotResample();
    virtual ~BigPotResample();
    static int convert(AVCodecContext* codecCtx,
                       AVFrame* frame,
                       int out_sample_format,
                       int out_sample_rate,
                       int out_channels,
                       uint8_t* out_buf);
};


#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
}

#include "FfmpegRAII.h"

class PotResample
{
public:
    //static PotResample pot_resample_;
    PotResample();
    virtual ~PotResample();

public:
    int convert(AVCodecContext* codec_ctx, AVFrame* frame, int out_sample_rate, int out_channels, uint8_t* out_buf);
    int out_sample_format_ = AV_SAMPLE_FMT_FLT;
    void setOutFormat(int f) { out_sample_format_ = f; }
};

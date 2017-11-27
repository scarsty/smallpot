#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
}

class PotResample
{
public:
    //static PotResample pot_resample_;
    PotResample();
    virtual ~PotResample();
public:
    int convert(AVCodecContext* codecCtx, AVFrame* frame, int out_sample_rate, int out_channels, uint8_t* out_buf);
    int out_sample_format_ = AV_SAMPLE_FMT_S16;
    //static PotResample* getInstance() { return &pot_resample_; }
    void setOutFormat(int f) { out_sample_format_ = f; }
};


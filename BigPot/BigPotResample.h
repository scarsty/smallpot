#pragma once

extern "C"
{
#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libswresample/swresample.h"
#include "libavutil/opt.h"
}

#define BP_AUDIO_RESAMPLE_FORMAT AV_SAMPLE_FMT_S16

class BigPotResample
{
public:
	BigPotResample();
	virtual ~BigPotResample();
	static int convert(AVCodecContext * codecCtx,
		AVFrame * frame,
		int out_sample_format,
		int out_channels,
		int out_sample_rate,
		uint8_t* out_buf);
};


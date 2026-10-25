#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
}

#include <memory>

struct AVCodecContextDeleter
{
    void operator()(AVCodecContext* context) const { avcodec_free_context(&context); }
};

struct AVFrameDeleter
{
    void operator()(AVFrame* frame) const { av_frame_free(&frame); }
};

struct AVFormatContextDeleter
{
    void operator()(AVFormatContext* context) const { avformat_close_input(&context); }
};

struct SwrContextDeleter
{
    void operator()(SwrContext* context) const { swr_free(&context); }
};

using AVCodecContextPtr = std::unique_ptr<AVCodecContext, AVCodecContextDeleter>;
using AVFramePtr = std::unique_ptr<AVFrame, AVFrameDeleter>;
using AVFormatContextPtr = std::unique_ptr<AVFormatContext, AVFormatContextDeleter>;
using SwrContextPtr = std::unique_ptr<SwrContext, SwrContextDeleter>;

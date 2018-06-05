#include "PotStreamVideo.h"


PotStreamVideo::PotStreamVideo()
{
    //视频缓冲区, 足够大时会较流畅，但是跳帧会闪烁
    type_ = BPMEDIA_TYPE_VIDEO;
}


PotStreamVideo::~PotStreamVideo()
{
}

//-1无视频
//1有可显示的包，未到时间
//2已经没有可显示的包
int PotStreamVideo::show(int time)
{
    if (stream_index_ < 0)
    {
        return NoVideo;
    }
    if (haveDecoded())
    {
        auto f = getCurrentContent();
        int time_c = f.time;
        if (time >= time_c)
        {
            auto tex = (BP_Texture*)f.data;
            engine_->renderCopy(tex);
            time_shown_ = time_c;
            ticks_shown_ = engine_->getTicks();
            dropDecoded();
            return VideoFrameShowed;
        }
        else
        {
            return VideoFrameBeforeTime;
        }
    }
    return NoVideoFrame;
}

int PotStreamVideo::getSDLPixFmt()
{
    std::map<int, int> pix_ffmpeg_sdl =
    {
        { AV_PIX_FMT_RGB8,           SDL_PIXELFORMAT_RGB332 },
        { AV_PIX_FMT_RGB444,         SDL_PIXELFORMAT_RGB444 },
        { AV_PIX_FMT_RGB555,         SDL_PIXELFORMAT_RGB555 },
        { AV_PIX_FMT_BGR555,         SDL_PIXELFORMAT_BGR555 },
        { AV_PIX_FMT_RGB565,         SDL_PIXELFORMAT_RGB565 },
        { AV_PIX_FMT_BGR565,         SDL_PIXELFORMAT_BGR565 },
        { AV_PIX_FMT_RGB24,          SDL_PIXELFORMAT_RGB24 },
        { AV_PIX_FMT_BGR24,          SDL_PIXELFORMAT_BGR24 },
        { AV_PIX_FMT_0RGB32,         SDL_PIXELFORMAT_RGB888 },
        { AV_PIX_FMT_0BGR32,         SDL_PIXELFORMAT_BGR888 },
        { AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
        { AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
        { AV_PIX_FMT_RGB32,          SDL_PIXELFORMAT_ARGB8888 },
        { AV_PIX_FMT_RGB32_1,        SDL_PIXELFORMAT_RGBA8888 },
        { AV_PIX_FMT_BGR32,          SDL_PIXELFORMAT_ABGR8888 },
        { AV_PIX_FMT_BGR32_1,        SDL_PIXELFORMAT_BGRA8888 },
        { AV_PIX_FMT_YUV420P,        SDL_PIXELFORMAT_IYUV },
        { AV_PIX_FMT_YUYV422,        SDL_PIXELFORMAT_YUY2 },
        { AV_PIX_FMT_UYVY422,        SDL_PIXELFORMAT_UYVY },
        { AV_PIX_FMT_NONE,           SDL_PIXELFORMAT_UNKNOWN },
    };
    return pix_ffmpeg_sdl[codec_ctx_->pix_fmt];
}

void PotStreamVideo::freeContent(void* p)
{
    engine_->destroyTexture((BP_Texture*)p);
}

FrameContent PotStreamVideo::convertFrameToContent()
{
    auto& f = frame_;
    auto tex = nullptr;
    //auto tex = engine_->createYUVTexture(codec_ctx_->width, codec_ctx_->height);
    engine_->updateYUVTexture(tex, f->data[0], f->linesize[0], f->data[1], f->linesize[1], f->data[2], f->linesize[2]);
    return { time_dts_, f->linesize[0], tex };
}



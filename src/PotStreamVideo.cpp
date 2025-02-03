#include "PotStreamVideo.h"
#include "Config.h"
#include "DynamicLibrary.h"

PotStreamVideo::PotStreamVideo()
{
    //视频缓冲区, 足够大时会较流畅，但是跳帧会闪烁
    type_ = MEDIA_TYPE_VIDEO;

    //create_module_ = (create_module_t)DynamicLibrary::getFunction(Config::getInstance()->getString("plugin"), "create_module");
    //if (create_module_)
    //{
    //    auto model = Config::getInstance()->getString("filepath") + Config::getInstance()->getString("model");
    //    auto bin = Config::getInstance()->getString("filepath") + Config::getInstance()->getString("bin");

    //    std::wstring modelw(model.begin(), model.end());
    //    std::wstring binw(bin.begin(), bin.end());

    //    plugin_ = create_module_(modelw.c_str(), binw.c_str());
    //    scale_ = Config::getInstance()->getInteger("scale");
    //    run_module_ = (run_module_t)DynamicLibrary::getFunction(Config::getInstance()->getString("plugin"), "run_module");
    //    destroy_module_ = (destroy_module_t)DynamicLibrary::getFunction(Config::getInstance()->getString("plugin"), "destroy_module");
    //}
    //else
    //{
    //    fmt1::print("Load %s failed!\n", Config::getInstance()->getString("plugin"));
    //}
}

PotStreamVideo::~PotStreamVideo()
{
    if (img_convert_ctx_)
    {
        sws_freeContext(img_convert_ctx_);
    }
    if (destroy_module_ && plugin_)
    {
        destroy_module_(plugin_);
    }
}

void PotStreamVideo::freeContent(void* p)
{
    if (p != engine_->getMainTexture())
    {
        engine_->destroyTexture((Texture*)p);
    }
}

FrameContent PotStreamVideo::convertFrameToContent()
{
    auto& f = frame_;
    int width = getWidth();
    int height = getHeight();
    engine_->resizeMainTexture(width, height);
    auto tex = engine_->getMainTexture();
    switch (texture_pix_fmt_)
    {
    case SDL_PIXELFORMAT_UNKNOWN:    //实际使用SDL_PIXELFORMAT_RGBA8888
    {
        uint8_t* pixels[4];
        int pitch[4];
        if (plugin_ == nullptr)
        {
            img_convert_ctx_ = sws_getCachedContext(img_convert_ctx_, f->width, f->height, AVPixelFormat(f->format), f->width, f->height, AV_PIX_FMT_RGB32_1, SWS_FAST_BILINEAR, NULL, NULL, NULL);
            if (engine_->lockTexture(tex, nullptr, (void**)pixels, pitch))
            {
                sws_scale(img_convert_ctx_, (const uint8_t* const*)f->data, f->linesize, 0, f->height, pixels, pitch);
                engine_->unlockTexture(tex);
            }
        }
        else
        {
            double scale = 1;
            img_convert_ctx_ = sws_getCachedContext(img_convert_ctx_, f->width, f->height, AVPixelFormat(f->format), f->width / scale, f->height / scale, AV_PIX_FMT_RGB32_1, SWS_FAST_BILINEAR, NULL, NULL, NULL);
            if (engine_->lockTexture(tex, nullptr, (void**)pixels, pitch))
            {
                std::vector<char> buffer(f->width * f->height * 3);
                uint8_t* pixels1[4];
                int pitch1[4];
                pixels1[0] = (uint8_t*)buffer.data();
                pitch1[0] = int(f->width / scale) * 3;
                sws_scale(img_convert_ctx_, (const uint8_t* const*)f->data, f->linesize, 0, f->height, pixels1, pitch1);
                run_module_(plugin_, f->width / scale, f->height / scale, 3, buffer.data(), (char*)pixels[0]);
                engine_->unlockTexture(tex);
            }
        }
    }
    break;
    case SDL_PIXELFORMAT_IYUV:
        if (f->linesize[0] > 0 && f->linesize[1] > 0 && f->linesize[2] > 0)
        {
            engine_->updateYUVTexture(tex, f->data[0], f->linesize[0], f->data[1], f->linesize[1], f->data[2], f->linesize[2]);
        }
        else if (f->linesize[0] < 0 && f->linesize[1] < 0 && f->linesize[2] < 0)
        {
            engine_->updateYUVTexture(tex,
                f->data[0] + f->linesize[0] * (f->height - 1), -f->linesize[0],
                f->data[1] + f->linesize[1] * (AV_CEIL_RSHIFT(f->height, 1) - 1), -f->linesize[1],
                f->data[2] + f->linesize[2] * (AV_CEIL_RSHIFT(f->height, 1) - 1), -f->linesize[2]);
        }
        else
        {
            fmt1::print(stderr, "Mixed negative and positive line sizes are not supported.\n");
        }
        break;
    default:
        if (f->linesize[0] < 0)
        {
            engine_->updateTexture(tex, f->data[0] + f->linesize[0] * (f->height - 1), -f->linesize[0]);
        }
        else
        {
            engine_->updateTexture(tex, f->data[0], f->linesize[0]);
        }
    }
    return { time_dts_, f->linesize[0], tex };
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
            auto tex = (Texture*)f.data;
            engine_->renderTexture(tex);
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

SDL_PixelFormat PotStreamVideo::getSDLPixFmt()
{
    if (!exist())
    {
        return SDL_PIXELFORMAT_UNKNOWN;
    }
    std::map<int, SDL_PixelFormat> pix_ffmpeg_sdl = {
        { AV_PIX_FMT_RGB8, SDL_PIXELFORMAT_RGB332 },
        { AV_PIX_FMT_RGB444, SDL_PIXELFORMAT_XRGB4444 },
        { AV_PIX_FMT_RGB555, SDL_PIXELFORMAT_XRGB1555 },
        { AV_PIX_FMT_BGR555, SDL_PIXELFORMAT_XBGR1555 },
        { AV_PIX_FMT_RGB565, SDL_PIXELFORMAT_RGB565 },
        { AV_PIX_FMT_BGR565, SDL_PIXELFORMAT_BGR565 },
        { AV_PIX_FMT_RGB24, SDL_PIXELFORMAT_RGB24 },
        { AV_PIX_FMT_BGR24, SDL_PIXELFORMAT_BGR24 },
        { AV_PIX_FMT_0RGB32, SDL_PIXELFORMAT_XRGB8888 },
        { AV_PIX_FMT_0BGR32, SDL_PIXELFORMAT_XBGR8888 },
        { AV_PIX_FMT_NE(RGB0, 0BGR), SDL_PIXELFORMAT_RGBX8888 },
        { AV_PIX_FMT_NE(BGR0, 0RGB), SDL_PIXELFORMAT_BGRX8888 },
        { AV_PIX_FMT_RGB32, SDL_PIXELFORMAT_ARGB8888 },
        { AV_PIX_FMT_RGB32_1, SDL_PIXELFORMAT_RGBA8888 },
        { AV_PIX_FMT_BGR32, SDL_PIXELFORMAT_ABGR8888 },
        { AV_PIX_FMT_BGR32_1, SDL_PIXELFORMAT_BGRA8888 },
        { AV_PIX_FMT_YUV420P, SDL_PIXELFORMAT_IYUV },
        { AV_PIX_FMT_YUYV422, SDL_PIXELFORMAT_YUY2 },
        { AV_PIX_FMT_UYVY422, SDL_PIXELFORMAT_UYVY },
        { AV_PIX_FMT_NONE, SDL_PIXELFORMAT_UNKNOWN },
    };
    auto r = SDL_PIXELFORMAT_UNKNOWN;
    if (plugin_ == nullptr)
    {
        if (codec_ctx_ && pix_ffmpeg_sdl.count(codec_ctx_->pix_fmt) > 0)
        {
            r = pix_ffmpeg_sdl[codec_ctx_->pix_fmt];
            fmt1::print("pixel format is {}\n", int(r));
        }
    }
    texture_pix_fmt_ = r;
    return r;
}

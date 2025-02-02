#pragma once

#include "PotStream.h"
extern "C"
{
#include "libswscale/swscale.h"
}

void* create_module(const wchar_t*, const wchar_t*);
void run_module(void* module, int w, int h, int c, const char* src, char* dst);
void destroy_module(void* module);

using create_module_t = decltype(&create_module);
using run_module_t = decltype(&run_module);
using destroy_module_t = decltype(&destroy_module);

class PotStreamVideo : public PotStream
{
public:
    PotStreamVideo();
    ~PotStreamVideo();
    enum
    {
        NoVideo = -1,
        VideoFrameShowed = 0,
        VideoFrameBeforeTime = 1,
        NoVideoFrame = 2,
    } VideoFrameState;

private:
    //void clear();
    virtual void freeContent(void* p) override;
    virtual FrameContent convertFrameToContent() override;

public:
    int show(int time);

    virtual int getWidth() { return exist() ? int(codec_ctx_->width / 1) * scale_ : 0; }
    virtual int getHeight() { return exist() ? int(codec_ctx_->height / 1) * scale_ : 0; }

public:
    SDL_PixelFormat getSDLPixFmt();

private:
    int texture_pix_fmt_;
    SwsContext* img_convert_ctx_ = nullptr;
    void* plugin_ = nullptr;
    create_module_t create_module_ = nullptr;
    run_module_t run_module_ = nullptr;
    destroy_module_t destroy_module_ = nullptr;
    double scale_ = 1;
};

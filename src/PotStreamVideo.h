#pragma once

#include "PotStream.h"
extern "C"
{
#include "libswscale/swscale.h"
}

void run_module_t(void* module, int w, int h, int c, const char* src, char* dst);

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

    virtual int getWidth() { return exist() ? int(codec_ctx_->width / scale_) * scale_ : 0; }
    virtual int getHeight() { return exist() ? int(codec_ctx_->height / scale_) * scale_ : 0; }

public:
    int getSDLPixFmt();

private:
    int texture_pix_fmt_;
    SwsContext* img_convert_ctx_ = nullptr;
    void* plugin_ = nullptr;
    void* create_module_ = nullptr;
    void* run_module_ = nullptr;
    double scale_ = 1;
};

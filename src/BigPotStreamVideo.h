#pragma once

#include "BigPotStream.h"

class BigPotStreamVideo : public BigPotStream
{
public:
    BigPotStreamVideo();
    ~BigPotStreamVideo();
    enum
    {
        NoVideo = -1,
        VideoFrameShowed = 0,
        VideoFrameBeforeTime = 1,
        NoVideoFrame = 2,
    } VideoFrameState;
private:
    //void clear();
    virtual void freeContent(void* p);
    virtual Content convertFrameToContent(void* p = nullptr);
public:
    int showTexture(int time);
    int dropTexture();
};

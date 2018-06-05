#pragma once

#include "PotStream.h"

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
    virtual int avcodec_decode_packet(AVCodecContext* cont, int* n, AVPacket* packet) override
    {
        return avcodec_decode_video2(cont, frame_, n, packet);
    }
public:
    int show(int time);
public:
    int getSDLPixFmt();
};

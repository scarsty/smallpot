#pragma once
#include "BigPotStream.h"

class BigPotStreamSubtitle : public BigPotStream
{
private:
    virtual int avcodec_decode_packet(AVCodecContext* cont, void* subtitle, int* n, AVPacket* packet) override
    { return avcodec_decode_subtitle2(cont, (AVSubtitle*)subtitle, n, packet); }
public:
    BigPotStreamSubtitle();
    virtual ~BigPotStreamSubtitle();
};


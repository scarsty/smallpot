#pragma once
#include "PotStream.h"

class PotStreamSubtitle : public PotStream
{
private:
    virtual int avcodec_decode_packet(AVCodecContext* cont, void* subtitle, int* n, AVPacket* packet) override
    { return avcodec_decode_subtitle2(cont, (AVSubtitle*)subtitle, n, packet); }
public:
    PotStreamSubtitle() {}
    virtual ~PotStreamSubtitle() {}
};


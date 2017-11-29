#pragma once
#include "PotStream.h"
#include "PotSubtitle.h"

class PotStreamSubtitle : public PotStream
{
private:
    AVSubtitle avsubtitle_;
    PotSubtitle* sub_ = nullptr;
    bool sub_inited_ = false;
public:
    PotStreamSubtitle();
    virtual ~PotStreamSubtitle();
    int show(int time);
    void setFrameSize(int w, int h);
    virtual int openFile(const std::string& filename);
    void clear();
private:
    virtual int avcodec_decode_packet(AVCodecContext* cont, int* n, AVPacket* packet) override
    {
        int ret = avcodec_decode_subtitle2(cont, &avsubtitle_, n, packet);
        return ret;
    }
    virtual FrameContent convertFrameToContent() override;
};


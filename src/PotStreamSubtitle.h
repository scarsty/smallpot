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
    void show(int time);
    void setFrameSize(int w, int h);
private:
    virtual int avcodec_decode_packet(AVCodecContext* cont, void* subtitle, int* n, AVPacket* packet) override;
    virtual Content convertFrameToContent(void* p = nullptr) override;
public:
    virtual int openFile(const std::string& filename);
};


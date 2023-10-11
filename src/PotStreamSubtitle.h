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
    virtual bool exist() { return PotStream::exist() && sub_ && sub_->exist(); }
private:
    virtual int avcodec_decode_packet(AVCodecContext* cont, int* n, AVPacket* packet) override;
    virtual FrameContent convertFrameToContent() override;
};


#pragma once

#include "PotStream.h"
#include "PotResample.h"

class PotStreamAudio : public PotStream
{
public:
    PotStreamAudio();
    virtual ~PotStreamAudio();
private:
    const int scream_size_ = 0x400000, convert_size_ = 192000;
    int volume_;
    int scream_length_ = 0;
    //uint32_t _readed = 0;
    uint8_t* resample_buffer_ = nullptr;
    int64_t data_read_ = 0, data_write_ = 0;  //读取和写入字节数，实际位置由该值与尺寸的余数计算
    int freq_, channels_;
    PotResample resample_;

    void mixAudioData(uint8_t* stream, int len);
    int closeAudioDevice();
    virtual Content convertFrameToContent(void* p = nullptr) override;
    virtual void freeContent(void* buffer) override;
    virtual bool needDecode2() override;

    virtual int avcodec_decode_packet(AVCodecContext* cont, void* frame, int* n, AVPacket* packet) override
    { return avcodec_decode_audio4(cont, (AVFrame*)frame, n, packet); }
public:
    void openAudioDevice();
    void resetDecodeState();
    int setVolume(int v);
    int changeVolume(int v);
    int getVolume() { return volume_; };
    virtual void setPause(bool pause) override;
};
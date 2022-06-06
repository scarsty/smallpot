#pragma once

#include "PotResample.h"
#include "PotStream.h"

/*
    这里假设音频流是连续的，并未考虑有一段缺失的情况
*/

class PotStreamAudio : public PotStream
{
public:
    PotStreamAudio();
    virtual ~PotStreamAudio();

private:
    const int buffer_size_ = 0x400000, convert_size_ = 192000;
    void* buffer_ = nullptr;
    static int volume_;
    int scream_length_ = 0;
    uint8_t* resample_buffer_ = nullptr;
    int64_t data_read_ = 0, data_write_ = 0;    //读取和写入字节数，实际位置由该值与尺寸的余数计算
    int freq_, channels_;
    PotResample resample_;

    void mixAudioData(uint8_t* stream, int len);
    virtual FrameContent convertFrameToContent() override;
    virtual bool needDecode2() override;

public:
    void openAudioDevice();
    int closeAudioDevice();
    void resetDecodeState();
    static int setVolume(int v);
    static int changeVolume(int v);
    static int getVolume() { return volume_; };
    virtual void setPause(bool pause) override;
};

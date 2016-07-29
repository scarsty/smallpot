#pragma once

#include "BigPotStream.h"
#include "BigPotResample.h"

class BigPotStreamAudio : public BigPotStream
{
public:
    BigPotStreamAudio();
    virtual ~BigPotStreamAudio();
private:
    const int _scream_size = 0x400000, _convert_size = 192000;
    int _volume;
    int _scream_length = 0;
    //uint32_t _readed = 0;
    uint8_t* _resample_buffer = nullptr;
    int64_t _data_read = 0, _data_write = 0;  //读取和写入字节数，实际位置由该值与尺寸的余数计算
    int _freq, _channels;

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
    int getVolume() { return _volume; };
    virtual void setPause(bool pause) override;
};
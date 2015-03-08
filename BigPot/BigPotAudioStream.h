#pragma once

#include "BigPotMediaStream.h"
#include "BigPotResample.h"

class BigPotAudioStream : public BigPotMediaStream
{
public:
	BigPotAudioStream();
	virtual ~BigPotAudioStream();
private:
	const int screamSize = 0x400000, convertSize = 192000;
	int volume;
	int screamLength = 0;
	uint32_t readed = 0;
	uint8_t* resampleBuffer = nullptr;
    int64_t dataRead = 0, dataWrite = 0;  //读取和写入字节数，实际位置由该值与尺寸的余数计算
	int freq, channels;

	void mixAudioData(uint8_t* stream, int len);
	int closeAudioDevice();
	virtual FrameData convert(void* p = nullptr);
	virtual void freeData(void* buffer);
	virtual bool needDecode2();
public:
	void openAudioDevice();
	void resetDecodeState();
	int setVolume(int v);
	int changeVolume(int v);
	bool setPause(bool pause);
};
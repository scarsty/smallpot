#pragma once

#include "BigPotMediaStream.h"

class BigPotVideoStream : public BigPotMediaStream
{
public:
	BigPotVideoStream();
	~BigPotVideoStream();
private:
	void clear();
	virtual void freeData(void* p);
	virtual FrameData convert(void* p = nullptr);
public:
	int showTexture(int time);
	int dropTexture();
};

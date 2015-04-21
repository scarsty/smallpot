#pragma once

#include "BigPotStream.h"

class BigPotStreamVideo : public BigPotStream
{
public:
	BigPotStreamVideo();
	~BigPotStreamVideo();
private:
	void clear();
	virtual void freeData(void* p);
	virtual FrameData convert(void* p = nullptr);
public:
	int showTexture(int time);
	int dropTexture();
};

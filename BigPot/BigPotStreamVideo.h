#pragma once

#include "BigPotStream.h"

class BigPotStreamVideo : public BigPotStream
{
public:
	BigPotStreamVideo();
	~BigPotStreamVideo();
private:
	void clear();
	virtual void freeContent(void* p);
	virtual ContentData convertFrameToContent(void* p = nullptr);
public:
	int showTexture(int time);
	int dropTexture();
};

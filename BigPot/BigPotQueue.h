#pragma once

#include "BigPotBase.h"
#include "BigPotMediaStream.h"

class BigPotQueue
{
public:
	const int maxCount = 100;
	BigPotMediaStream* mediaInfo;
	int w, h;
	int timeBegin = -1;
	int time;

public:
	BigPotQueue();
	virtual ~BigPotQueue();

	void init(BigPotMediaStream* mediaInfo);
	void resetTimeBegin();
};

class BigPotVideoQueue : public BigPotQueue
{
public:
	BigPotVideoQueue(){};
	virtual ~BigPotVideoQueue(){};

	SDL_Renderer* ren;
	SDL_Texture* tex;
	bool decoded = false;
	
	void setRendererTexture(SDL_Renderer* ren, SDL_Texture* tex)
	{
		this->ren = ren; this->tex = tex;
	}

	void setDecoded(bool b)
	{
		decoded = b;
	}

	int tryDecodeTexture();
	int showTexture(int time);
	int dropTexture();
	void clear();
};

class BigPotAudioQueue : public BigPotQueue
{
public:
	BigPotAudioQueue(){};
	virtual ~BigPotAudioQueue(){};
	HSTREAM h;

	void setHStream(HSTREAM h)
	{
		this->h = h;
	};

	int decodeFrame();

	int getTime();
};




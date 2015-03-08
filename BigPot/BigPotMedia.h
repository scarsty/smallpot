#pragma once

#include "BigPotBase.h"
#include "BigPotMediaStream.h"
#include "BigPotAudioStream.h"
#include "BigPotVideoStream.h"


class BigPotMedia : public BigPotBase
{
public:
	BigPotMedia();
	virtual ~BigPotMedia();

	BigPotVideoStream *videoStream;
	BigPotAudioStream *audioStream;
private:
	int count = 0;
	int totalTime = 0;
	int lastdts = 0;
	int timebase = 0;
	bool seeking = false;
public:
	int decodeFrame();
	int openFile(const string &filename);
	int getAudioTime();
	int getVideoTime();
	int seekTime(int time, int direct = 1);
	int seekPos(double pos);
	int showVideoFrame(int time);
	int getTotalTime() { return totalTime; }
	int getTime();
	void destroy();
};


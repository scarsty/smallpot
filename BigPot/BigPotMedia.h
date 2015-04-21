#pragma once

#include "BigPotBase.h"
#include "BigPotStream.h"
#include "BigPotStreamAudio.h"
#include "BigPotStreamVideo.h"
#include "BigPotString.h"

using namespace BigPotString;
//#include "BigPotSubtitle.h"


class BigPotMedia : public BigPotBase
{
public:
	BigPotMedia();
	virtual ~BigPotMedia();
private:
	BigPotStreamVideo *_videoStream;
	BigPotStreamAudio *_audioStream;
private:
	int _count = 0;
	int _totalTime = 0;
	int _lastdts = 0;
	int _timebase = 0;
	bool _seeking = false;
public:
	BigPotStreamVideo *getVideoStream(){ return _videoStream; };
	BigPotStreamAudio *getAudioStream(){ return _audioStream; };
	int decodeFrame();
	int openFile(const string &filename);
	int getAudioTime();
	int getVideoTime();
	int seekTime(int time, int direct = 1);
	int seekPos(double pos);
	int showVideoFrame(int time);
	int getTotalTime() { return _totalTime; }
	int getTime();
	void destroy();
	bool isMedia()
	{
		return _audioStream->exist() || _videoStream->exist();
	}
	void setPause(bool pause);
};


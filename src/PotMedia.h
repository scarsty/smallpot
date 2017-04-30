#pragma once

#include "PotBase.h"
#include "PotStream.h"
#include "PotStreamAudio.h"
#include "PotStreamVideo.h"
#include "PotStreamSubtitle.h"

//#include "BigPotSubtitle.h"


class PotMedia : public PotBase
{
public:
    PotMedia();
    virtual ~PotMedia();
private:
    PotStreamVideo* _streamVideo = nullptr;
    PotStreamAudio* _streamAudio = nullptr;
    PotStreamSubtitle* _streamSubtitle = nullptr;
    int _extAudioFrame = 1; //额外解压一帧音频，有时视频尺寸很大，可能导致音频解码过慢
private:
    int _count = 0;
    int _totalTime = 0;
    int _lastdts = 0;
    int _timebase = 0;
    bool _seeking = false;
public:
    PotStreamVideo* getVideo() { return _streamVideo; };
    PotStreamAudio* getAudio() { return _streamAudio; };
    int decodeFrame();
    int openFile(const std::string& filename);
    int getAudioTime();
    int getVideoTime();
    int seekTime(int time, int direct = 1, int reset = 0);
    int seekPos(double pos, int direct = 1, int reset = 0);
    int showVideoFrame(int time);
    int getTotalTime() { return _totalTime; }
    int getTime();
    void destroy();
    bool isMedia();
    void setPause(bool pause);
};


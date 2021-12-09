#pragma once

#include "PotBase.h"
#include "PotStream.h"
#include "PotStreamAudio.h"
#include "PotStreamSubtitle.h"
#include "PotStreamVideo.h"

/*
    一个媒体文件包含3类流：视频，音频，字幕
    视频流通常只有一个，而另外两类流
    这里使用3个流来同时打开文件，其中音频和字幕可以通过修改索引来切换
*/

class PotMedia : public PotBase
{
public:
    PotMedia();
    virtual ~PotMedia();

private:
    std::vector<PotStream*> streams_;

    AVFormatContext* format_ctx_video_, * format_ctx_audio_, * format_ctx_subtitle_;

    PotStreamVideo* stream_video_ = nullptr;
    PotStreamAudio* stream_audio_ = nullptr;
    PotStreamSubtitle* stream_subtitle_ = nullptr;

    PotStreamVideo blank_video_;
    PotStreamAudio blank_audio_;
    PotStreamSubtitle blank_subtitle_;

private:
    int count_ = 0;
    int total_time_ = 0;
    int lastdts_ = 0;
    int timebase_ = 0;
    bool seeking_ = false;

public:
    PotStreamVideo* getVideo() { return stream_video_; };
    PotStreamAudio* getAudio() { return stream_audio_; };
    PotStreamSubtitle* getSubtitle() { return stream_subtitle_; };
    int decodeFrame();
    int openFile(const std::string& filename);
    int getAudioTime();
    int getVideoTime();
    int seekTime(int time, int direct = 1, int reset = 0);
    int seekPos(double pos, int direct = 1, int reset = 0);
    int showVideoFrame(int time);
    int getTotalTime() { return total_time_; }
    int getTime();
    void destroy();
    bool isMedia();
    void setPause(bool pause);
    void switchStream(PotMediaType mt);
    int getStreamCount(PotMediaType mt);
};

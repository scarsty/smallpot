#pragma once

#include "PotBase.h"
#include "PotStream.h"
#include "PotStreamAudio.h"
#include "PotStreamVideo.h"
#include <deque>
#include <unordered_map>

#ifndef WITHOUT_SUBTITLE
#include "PotStreamSubtitle.h"
#endif

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

    AVFormatContextPtr format_ctx_;

    PotStreamVideo* stream_video_ = nullptr;
    PotStreamAudio* stream_audio_ = nullptr;
#ifndef WITHOUT_SUBTITLE
    PotStreamSubtitle* stream_subtitle_ = nullptr;
#endif

    PotStreamVideo blank_video_;
    PotStreamAudio blank_audio_;
#ifndef WITHOUT_SUBTITLE
    PotStreamSubtitle blank_subtitle_;
#endif

private:
    int count_ = 0;
    int total_time_ = 0;
    int lastdts_ = 0;
    int timebase_ = 0;
    bool seeking_ = false;
    bool seek_request_pending_ = false;
    int pending_seek_time_ = 0;
    int pending_seek_direct_ = 1;
    int pending_seek_reset_ = 0;
    bool seek_decode_to_target_ = false;
    int seek_target_time_ = 0;
    int seek_stream_index_ = -1;
    std::deque<AVPacket> packet_queue_;
    int readPacket(int stream_index, AVPacket* packet);
    int seekDemux(int time, int direct, int reset);

public:
    PotStreamVideo* getVideo() { return stream_video_; }

    PotStreamAudio* getAudio() { return stream_audio_; }
#ifndef WITHOUT_SUBTITLE
    PotStreamSubtitle* getSubtitle() { return stream_subtitle_; }
#endif
    int decodeFrame();
    int openFile(const std::string& filename);
    int getAudioTime();
    int getVideoTime();
    int seekTime(int time, int direct = 1, int reset = 0);
    int seekPos(double pos, int direct = 1, int reset = 0);
    int showVideoFrame(int time);

    int getTotalTime() { return total_time_; }

    int getTime();
    bool isSeeking() const { return seeking_ || seek_request_pending_; }
    void requestSeek(int time, int direct = 1, int reset = 0);
    void finishSeekAfterFrameShown();
    void processSeekStep();
    void destroy();
    bool isMedia();
    void setPause(bool pause);
    void switchStream(PotMediaType mt);
    int getStreamCount(PotMediaType mt);
};

#pragma once

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
}

#include "PotBase.h"
#include "Engine.h"
#include <algorithm>
#include <map>
#include <mutex>

enum PotMediaType
{
    BPMEDIA_TYPE_VIDEO = AVMEDIA_TYPE_VIDEO,
    BPMEDIA_TYPE_AUDIO = AVMEDIA_TYPE_AUDIO,
    BPMEDIA_TYPE_SUBTITLE = AVMEDIA_TYPE_SUBTITLE,
};

/*
Context - 未解码数据
Packet - 读取的一个未解的包
Frame - 解得的一帧数据
Content - 转换而得的可以直接显示或播放的数据，包含时间，信息（通常为总字节），和指向数据区的指针
*/

struct FrameContent
{
    int time;
    int64_t info;
    void* data;
};

class PotStream : public PotBase
{
public:
    std::map<int, FrameContent> data_map_;    //解压出的内容的map，key是时间
    std::vector<int> stream_index_vector_;

    PotStream();
    virtual ~PotStream();
protected:
    PotMediaType type_;
    AVFormatContext* format_ctx_ = nullptr;
    AVFrame* frame_ = nullptr;
    AVStream* stream_ = nullptr;
    AVCodecContext* codec_ctx_ = nullptr;
    AVCodec* codec_ = nullptr;
    AVPacket packet_;
    bool need_read_packet_ = true;
    int stream_index_ = -1;
    double time_per_frame_ = 0, time_base_packet_ = 0;
    int max_size_ = 1;  //为0时仅预解一帧, 理论效果与=1相同, 但不使用map和附加缓冲区
    std::string filename_;

    int ticks_shown_ = -1;  //最近展示的ticks
    int time_dts_ = 0, time_pts_ = 0, time_shown_ = 0;  //解压时间，应展示时间，最近已经展示的时间
    int time_other_ = 0;
    int start_time_ = 0;
    int total_time_ = 0;

    bool pause_ = false;
    int pause_time_ = 0;
    bool key_frame_ = false;
    int data_length_ = 0;
    bool stopping = false;  //表示放弃继续解压这个流
    int decode_frame_count_ = 1;
    //int frame_number_;
private:
    bool decoded_ = false, skip_ = false, ended_ = false, seeking_ = false;
    int seek_record_ = 0;  //上次seek的记录
    virtual int avcodec_decode_packet(AVCodecContext* ctx, int* n, AVPacket* packet);
private:
    virtual FrameContent convertFrameToContent() { return { 0, 0, nullptr }; }
    int dropContent();
    void setMap(int key, FrameContent f);
    virtual void freeContent(void* p) {};
    void clearMap();
    bool needDecode();
    virtual bool needDecode2() { return true; };
    virtual int decodeNextPacketToFrame(bool decode, bool til_got);
protected:
    void setDecoded(bool b);
    bool haveDecoded();
    void dropAllDecoded();
    FrameContent getCurrentContent();
public:
    virtual int openFile(const std::string& filename);
    int tryDecodeFrame(bool reset = false);
    void dropDecoded();
    int getTotalTime();

    void setSkip(bool b) { skip_ = b; }
    void resetTimeBegin() { ticks_shown_ = -1; }

    int seek(int time, int direct = 1, int reset = 0);
    void setFrameTime();
    int getTime();
    int setAnotherTime(int time);
    int skipFrame(int time);

    void getSize(int& w, int& h);
    int getWidth() { return exist() ? codec_ctx_->width : 0; }
    int getHeight() { return exist() ? codec_ctx_->height : 0; }
    int getTimedts() { return time_dts_ > 0 ? time_dts_ : time_pts_; }
    int getTimeShown() { return time_shown_; }
    bool exist() { return this != nullptr && stream_index_ >= 0; }
    void resetTimeAxis(int time);
    bool isPause() { return pause_; }
    bool isKeyFrame() { return key_frame_; }
    virtual void setPause(bool pause);
    void resetDecoderState() { avcodec_flush_buffers(codec_ctx_); }
    double getRotation();
    void getRatio(int& x, int& y);
    int getRatioX() { return exist() ? std::max(stream_->sample_aspect_ratio.num, 1) : 1; }
    int getRatioY() { return exist() ? std::max(stream_->sample_aspect_ratio.den, 1) : 1; }
    bool isStopping() { return stopping; }

    //void setStreamIndex(int stream_index) { stream_index_ = stream_index; }
    int getStreamIndex() { return stream_index_; }
    void switchStream();

};



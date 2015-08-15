#pragma once

extern "C"
{
#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libavutil/avutil.h"
#include "ffmpeg/libavformat/avformat.h"
}

#include "BigPotBase.h"
#include "BigPotEngine.h"
#include <algorithm>
#include <map>

#ifndef __MINGW32__
#include <mutex> 
#endif
enum BigPotMediaType
{
	BPMEDIA_TYPE_VIDEO = AVMEDIA_TYPE_VIDEO,
	BPMEDIA_TYPE_AUDIO = AVMEDIA_TYPE_AUDIO,
};

/*
Context - 未解码数据
Packet - 读取的一个未解的包
Frame - 解得的一帧数据
Content - 转换而得的可以直接显示或播放的数据，包含时间，信息（通常为总字节），和指向数据区的指针
*/

class BigPotStream : public BigPotBase
{
public:
	struct Content
	{
		int time;
		int64_t info;
		void* data;
	};

	BigPotStream();
	virtual ~BigPotStream();
protected:
	BigPotMediaType type_;
	AVFormatContext* formatCtx_;
	AVStream* stream_;
	AVCodecContext *codecCtx_;
	AVCodec *codec_;
	AVPacket packet_;
	bool needReadPacket_ = true;
	int stream_index_ = -1;
	int decodeSizeInPacket_ = 0;
	double time_per_frame_ = 0, time_base_packet_ = 0;
	int maxSize_ = 0;  //为0时仅预解一帧, 理论效果与=1相同, 但不使用map和附加缓冲区
	AVFrame *frame_;
	string filename_;
	mutex mutex_;
	
	int ticks_shown_ = -1;
	int time_dts_ = 0, time_pts_ = 0, time_shown_ = 0;  //解压时间，应展示时间，最近已经展示的时间
	int time_other_ = 0;
	int start_time_ = 0;
	int total_time_ = 0;

	bool pause_ = false;
	int pause_time_ = 0;
	bool key_frame_ = false;
	void* data_ = nullptr;	//无缓冲时的用户数据, 可能为纹理或音频缓冲区
	int data_length_ = 0;
	//int frame_number_;
private:

	map<int, Content> _map;
	bool _decoded = false, _skip = false, _ended = false, _seeking = false;	
	int _seek_record = 0;  //上次seek的记录
	int(*avcodec_decode_packet)(AVCodecContext*, AVFrame*, int*, const AVPacket*) = nullptr;

private:
	virtual Content convertFrameToContent(void * p = nullptr) 
	{
		return{0, 0, nullptr};
	}

	int dropContent(int key = -1);
	void setMap(int key, Content f);
	virtual void freeContent(void* p){};
	void clearMap();	
	bool needDecode();
	virtual bool needDecode2() { return true; };
	int decodeNextPacketToFrame(bool decode = true);
protected:
	void setDecoded(bool b);
	bool haveDecoded();
	void dropAllDecoded();
	bool useMap();
	Content getCurrentContent();
public:
	int openFile(const string & filename);
	int tryDecodeFrame(bool reset = false);
	void dropDecoded();
	int getTotalTime();

	void setSkip(bool b)
	{
		_skip = b;
	}

	void resetTimeBegin()
	{
		ticks_shown_ = -1;
	}

	int seek(int time, int direct = 1, bool reset = false);
	void setFrameTime();
	int getTime();
	int setAnotherTime(int time);
	int skipFrame(int time);
	__declspec(deprecated)
		void getSize(int &w, int&h);
	int getWidth() { return exist() ? codecCtx_->width : 0; }
	int getHeight() { return exist() ? codecCtx_->height : 0; }
	int getTimedts()
	{
		return time_dts_>0?time_dts_:time_pts_;
	}
	int getTimeShown()
	{
		return time_shown_;
	}
	bool exist(){ return stream_index_ >= 0; }
	void resetTimeAxis(int time);
	bool isPause() { return pause_; }
	bool isKeyFrame() { return key_frame_; }
	virtual void setPause(bool pause);
	void resetDecoderState() { avcodec_flush_buffers(codecCtx_); }
	double getRotation();
	void getRatio(int &x, int &y);
	int getRatioX() { return exist() ? stream_->sample_aspect_ratio.num : 1; }
	int getRatioY() { return exist() ? stream_->sample_aspect_ratio.den : 1; }
};



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

class BigPotStream : public BigPotBase
{
public:
	struct FrameData
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
	int stream_index_ = -1;
	double time_per_frame_ = 0, time_per_packet_ = 0;
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
	uint32_t data_length_ = 0;
private:

	map<int, FrameData> _map;
	bool _decoded = false, _skip = false, _ended = false, _seeking = false;

private:
	virtual FrameData convert(void * p = nullptr) 
	{
		return{0, 0, nullptr};
	}

	int dropFrameData(int key = -1);
	void setMap(int key, FrameData f);
	virtual void freeData(void* p){};
	void clearMap();	
	bool needDecode();
	virtual bool needDecode2() { return true; };
protected:
	void setDecoded(bool b);
	bool haveDecoded();
	void dropAllDecoded();
	bool useMap();
	FrameData getCurrentFrameData();
public:
	int openFile(const string & filename, BigPotMediaType type);
	int decodeFramePre(bool decode = true);
	int decodeFrame(bool reset = false);
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

	int seek(int time, int direct = 1);
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

	int frame_number_;

};



#include "BigPotMediaStream.h"
#include "BigPotResample.h"

BigPotMediaStream::BigPotMediaStream()
{
	av_register_all();
	formatCtx_ = avformat_alloc_context();
	frame_ = av_frame_alloc();
	//mutex_cpp.;
	time_shown_ = 0;
	ticks_shown_ = engine_->getTicks();
}


BigPotMediaStream::~BigPotMediaStream()
{
	av_frame_free(&frame_);
	avformat_close_input(&formatCtx_);
	clearMap();
	stream_index_ = -1;
	//DestroyMutex(mutex_cpp);
}

//返回为非负才正常
int BigPotMediaStream::openFile(const string & filename, BigPotMediaType type)
{
	stream_index_ = -1;
	this->filename_ = filename;
	if (avformat_open_input(&formatCtx_, filename.c_str(), nullptr, nullptr) == 0)
	{
		avformat_find_stream_info(formatCtx_, nullptr);
		this->type_ = type;
		for (int i = 0; i < formatCtx_->nb_streams; ++i)
		{
			if (formatCtx_->streams[i]->codec->codec_type == type)
			{
				//printf("finded media stream: %d\n", type);
				stream_ = formatCtx_->streams[i];
				codecCtx_ = stream_->codec;
				//timebase = av_q2d(formatCtx->streams[i]->time_base);
				if (stream_->r_frame_rate.den)
					time_per_frame_ = 1e3 / av_q2d(stream_->r_frame_rate);
				time_per_packet_ = 1e3*av_q2d(stream_->time_base);
				total_time_ = formatCtx_->duration *1e3 / AV_TIME_BASE;
				start_time_ = formatCtx_->start_time *1e3 / AV_TIME_BASE;
				//totalTime = (int)stream->nb_frames * timePerFrame;
				stream_index_ = i;
				codec_ = avcodec_find_decoder(codecCtx_->codec_id);
				avcodec_open2(codecCtx_, codec_, nullptr);
				break;
			}
		}
	}
	return stream_index_;
}

//解压帧，同时会更新当前的时间戳
int BigPotMediaStream::decodeFramePre()
{
	//3个状态，为正表示解到帧，为0表示还有可能解到帧，为负表示已经无帧
	int ret = 0;
	while (ret==0)
	{
		//auto packet = new AVPacket;
		if (av_read_frame(formatCtx_, &packet_) >= 0)
		{
			if (packet_.stream_index == stream_index_)
			{
				switch (type_)
				{
				case BPMEDIA_TYPE_VIDEO:
					avcodec_decode_video2(codecCtx_, frame_, &ret, &packet_);
					break;
				case BPMEDIA_TYPE_AUDIO:
					avcodec_decode_audio4(codecCtx_, frame_, &ret, &packet_);
					break;
				}
			}
			_ended = false;
		}
		else
		{
			ret = -1;
			_ended = true;
			break;
		}
		if (ret > 0)
		{
			time_pts_ = packet_.pts * time_per_packet_;
			time_dts_ = packet_.dts * time_per_packet_;
		}
		av_free_packet(&packet_);
	}
	return ret;
}

int BigPotMediaStream::decodeFrame()
{
	if (exist() && needDecode() && decodeFramePre() > 0)
	{
		auto f = convert();
		if (useMap())
		{
			if (_map.count(f.time) == 0 && f.data)
				_map[f.time] = f;
		}
		else
		{

		}
		setDecoded(true);
		return 0;
	}
	return 1;
}


int BigPotMediaStream::getTotalTime()
{
	return total_time_;
}

int BigPotMediaStream::seek(int time, int direct)
{
	if (exist())
	{
		clearMap();
		int c = 5;
		int64_t i = time / 1e3 * AV_TIME_BASE;
		int flag = AVSEEK_FLAG_FRAME;
		if (direct < 0)
			flag = flag | AVSEEK_FLAG_BACKWARD;
		av_seek_frame(formatCtx_, -1, i, flag);
		setDecoded(false);
	}
	return 0;
}

int BigPotMediaStream::dropFrameData(int key)
{
	mutex_.lock();
	if (_map.size() > 0)
	{
		auto p = _map.begin()->second.data;
		if (p)
		{
			freeData(p);
		}
		_map.erase(_map.begin());
	}
	mutex_.unlock();
	return 0;
}

void BigPotMediaStream::clearMap()
{
	//SDL_LockMutex(mutex_cpp);
	//printf("clear buffer begin with %d\n", _map.size());
	//for (auto i = _map.begin(); i != _map.end(); i++)
	mutex_.lock();
	for (auto &i : _map)
	{
		freeData(i.second.data);
	}
	_map.clear();
	mutex_.unlock();
	//printf("clear buffer end with %d\n", _map.size());
	//SDL_UnlockMutex(mutex_cpp);
}

void BigPotMediaStream::setMap(int key, FrameData f)
{
	_map[key] = f;
}

bool BigPotMediaStream::needDecode()
{
	if (!needDecode2())
		return false;
	if (useMap())
		return (_map.size() < maxSize_);
	else
		return !_decoded;
}

void BigPotMediaStream::setDecoded(bool b)
{
	_decoded = b;
}

void BigPotMediaStream::dropDecoded()
{
	if (useMap())
		dropFrameData();
	else
		_decoded = false;
}

bool BigPotMediaStream::useMap()
{
	return maxSize_ > 0;
}

BigPotMediaStream::FrameData BigPotMediaStream::getCurrentFrameData()
{
	if (useMap())
	{
		if (_map.size() > 0)
			return _map.begin()->second;
		else
			return{ -1, -1, nullptr };
	}
	else
	{
		return{ time_dts_, data_length_, data_ };
	}
}

bool BigPotMediaStream::haveDecoded()
{
	if (useMap())
	{
		return _map.size() > 0;
	}
	else
	{
		return _decoded;
	}
}


int BigPotMediaStream::getTime()
{
	//if (type_== BPMEDIA_TYPE_AUDIO)
	    //printf("%d//%d//%d//\n", time_shown_, ticks_shown_, engine_->getTicks());
	if (exist() && !_ended)
		return time_shown_ - ticks_shown_ + engine_->getTicks();
	else
		return total_time_;
}

int BigPotMediaStream::setAnotherTime(int time)
{
	return time_other_ = time;
}

int BigPotMediaStream::skipFrame(int time)
{
	int n = 0;
	while (time_dts_ < time)
	{
		n++;
		if (decodeFramePre() < 0)
			break;
	}
	return n;
}

void BigPotMediaStream::getSize(int &w, int&h)
{
	if (exist())
	{
		w = codecCtx_->width;
		h = codecCtx_->height;
	}
}



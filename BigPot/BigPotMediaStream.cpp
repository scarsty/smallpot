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
int BigPotMediaStream::decodeFramePre(bool decode /*= true*/)
{
	//3个状态，为正表示解到帧，为0表示还有可能解到帧，为负表示已经无帧
	if (!exist()) return -2;
	int ret = 0;
	//cout << "depre "<<engine_->getTicks() << " ";
	while (ret==0)
	{
		//auto packet = new AVPacket;
		if (av_read_frame(formatCtx_, &packet_) >= 0)
		{
			if (packet_.stream_index == stream_index_)
			{
				if (decode)
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
				else
				{
					ret = 2;
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
			//key_frame_ = frame_->key_frame;
			//frame_number_ = codecCtx_->frame_number;
			//if (type_ == 0 && key_frame_)printf("\n%dis key\n", time_dts_);
		}
		av_free_packet(&packet_);
	}
	//cout << engine_->getTicks() << '\n';
	return ret;
}


//参数为是否重置暂停时间和显示时间，一般seek后应立刻重置
int BigPotMediaStream::decodeFrame(bool reset)
{
	if (exist() && needDecode() && decodeFramePre() > 0)
	{
		auto f = convert();
		if (useMap())
		{
			//如果只有一帧，则静止时间需更新
			if (_map.size() == 0)
			{
				if (reset)
					resetTimeAxis(time_dts_);
			}
			if (_map.count(f.time) == 0 && f.data)
				_map[f.time] = f;
		}
		else
		{
			if (reset)
				resetTimeAxis(time_dts_);
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
		int c = 5;
		int64_t i = time / 1e3 * AV_TIME_BASE;

		int flag = 0;
		if (direct < 0)
			flag = flag | AVSEEK_FLAG_BACKWARD;
		av_seek_frame(formatCtx_, -1, i, flag);
		if (pause_)
			avcodec_flush_buffers(codecCtx_);
		dropAllDecoded();
		//decodeFrame(true);
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
	if (pause_)
		return pause_time_;
	//if (type_== BPMEDIA_TYPE_AUDIO)
	    //printf("%d//%d//%d//\n", time_shown_, ticks_shown_, engine_->getTicks());
    //if (exist() && !_ended)
    if (exist())
		return min(int(time_shown_ - ticks_shown_ + engine_->getTicks()), total_time_);
    return 0;
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
		//视频需解码，因为关键帧不解后续一系列都有问题，音频可以只读不解
		if (decodeFramePre(type_ == BPMEDIA_TYPE_VIDEO) < 0)
			break;
	}
	//跳帧后需丢弃原来的解码，重置时间轴
	dropAllDecoded();
	resetTimeAxis(time_dts_);
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

void BigPotMediaStream::dropAllDecoded()
{
	clearMap();
	setDecoded(false);
}

void BigPotMediaStream::setPause(bool pause)
{
	pause_ = pause;
	pause_time_ = getTime();
}

void BigPotMediaStream::resetTimeAxis(int time)
{
	pause_time_ = time_shown_ = time;
	ticks_shown_ = engine_->getTicks();
}



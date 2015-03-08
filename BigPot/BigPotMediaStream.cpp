#include "BigPotMediaStream.h"
#include "BigPotResample.h"

BigPotMediaStream::BigPotMediaStream()
{
	av_register_all();
	formatCtx = avformat_alloc_context();
	frame = av_frame_alloc();
	//mutex_cpp.;
	timeShown = 0;
	ticksShown = engine->getTicks();
}


BigPotMediaStream::~BigPotMediaStream()
{
	av_frame_free(&frame);
	avformat_close_input(&formatCtx);
	clearMap();
	streamIndex = -1;
	//DestroyMutex(mutex_cpp);
}

//返回为非负才正常
int BigPotMediaStream::openFile(const string & filename, BigPotMediaType type)
{
	streamIndex = -1;
	this->filename = filename;
	if (avformat_open_input(&formatCtx, filename.c_str(), nullptr, nullptr) == 0)
	{
		avformat_find_stream_info(formatCtx, nullptr);
		this->type = type;
		for (int i = 0; i < formatCtx->nb_streams; ++i)
		{
			if (formatCtx->streams[i]->codec->codec_type == type)
			{
				//printf("finded media stream: %d\n", type);
				stream = formatCtx->streams[i];
				codecCtx = stream->codec;
				//timebase = av_q2d(formatCtx->streams[i]->time_base);
				if (stream->r_frame_rate.den)
					timePerFrame = 1e3 / av_q2d(stream->r_frame_rate);
				timePerPacket = 1e3*av_q2d(stream->time_base);
				totalTime = formatCtx->duration *1e3 / AV_TIME_BASE;
				startTime = formatCtx->start_time *1e3 / AV_TIME_BASE;
				//totalTime = (int)stream->nb_frames * timePerFrame;
				streamIndex = i;
				codec = avcodec_find_decoder(codecCtx->codec_id);
				avcodec_open2(codecCtx, codec, nullptr);
				break;
			}
		}
	}
	return streamIndex;
}

//解压帧，同时会更新当前的时间戳
int BigPotMediaStream::decodeFramePre()
{
	//3个状态，为正表示解到帧，为0表示还有可能解到帧，为负表示已经无帧
	int ret = 0;
	while (ret==0)
	{
		//auto packet = new AVPacket;
		if (av_read_frame(formatCtx, &packet) >= 0)
		{
			if (packet.stream_index == streamIndex)
			{
				switch (type)
				{
				case BPMEDIA_TYPE_VIDEO:
					avcodec_decode_video2(codecCtx, frame, &ret, &packet);
					break;
				case BPMEDIA_TYPE_AUDIO:
					avcodec_decode_audio4(codecCtx, frame, &ret, &packet);
					break;
				}
			}
			ended = false;
		}
		else
		{
			ret = -1;
			ended = true;
			break;
		}
		if (ret > 0)
		{
			timepts = packet.pts * timePerPacket;
			timedts = packet.dts * timePerPacket;
		}
		av_free_packet(&packet);
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
	return totalTime;
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
		av_seek_frame(formatCtx, -1, i, flag);
		setDecoded(false);
	}
	return 0;
}

int BigPotMediaStream::dropFrameData(int key)
{
	mutex_cpp.lock();
	if (_map.size() > 0)
	{
		auto p = _map.begin()->second.data;
		if (p)
		{
			freeData(p);
		}
		_map.erase(_map.begin());
	}
	mutex_cpp.unlock();
	return 0;
}

void BigPotMediaStream::clearMap()
{
	//SDL_LockMutex(mutex_cpp);
	//printf("clear buffer begin with %d\n", _map.size());
	//for (auto i = _map.begin(); i != _map.end(); i++)
	mutex_cpp.lock();
	for (auto &i : _map)
	{
		freeData(i.second.data);
	}
	_map.clear();
	mutex_cpp.unlock();
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
		return (_map.size() < maxSize);
	else
		return !decoded;
}

void BigPotMediaStream::setDecoded(bool b)
{
	decoded = b;
}

void BigPotMediaStream::dropDecoded()
{
	if (useMap())
		dropFrameData();
	else
		decoded = false;
}

bool BigPotMediaStream::useMap()
{
	return maxSize > 0;
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
		return{ timedts, datalength, data };
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
		return decoded;
	}
}


int BigPotMediaStream::getTime()
{
	if (exist() && !ended)
		return timeShown - ticksShown + engine->getTicks();
	else
		return totalTime;
}

int BigPotMediaStream::setAnotherTime(int time)
{
	return time_another = time;
}

int BigPotMediaStream::skipFrame(int time)
{
	int n = 0;
	while (timedts < time)
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
		w = codecCtx->width;
		h = codecCtx->height;
	}
}



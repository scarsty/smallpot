#include "BigPotAudioStream.h"

BigPotAudioStream::BigPotAudioStream()
{
	volume = control->getMaxVolume() / 2;
	//预解包数量
	//除非知道音频包定长，否则不应设为0，一般情况下都不建议为0
	maxSize = 100;
	//缓冲区大小4M保存
	if (useMap()) 
		data = av_mallocz(screamSize);
	resampleBuffer =  (decltype(resampleBuffer))av_mallocz(convertSize);
}


BigPotAudioStream::~BigPotAudioStream()
{
	if (useMap())
		av_free(data);
	if (resampleBuffer)
		av_free(resampleBuffer);
}

void BigPotAudioStream::openAudioDevice()
{
	if (streamIndex < 0)
		return;
	freq = codecCtx->sample_rate;
	channels = codecCtx->channels;
	control->openAudio(freq, channels, codecCtx->frame_size,
		2048, bind(&BigPotAudioStream::mixAudioData, this, placeholders::_1, placeholders::_2));
}

int BigPotAudioStream::closeAudioDevice()
{
	control->closeAudio();
	return 0;
}

void BigPotAudioStream::mixAudioData(Uint8* stream, int len)
{
	if (!useMap())
	{
		control->mixAudio(stream, (uint8_t*)resampleBuffer, len, volume);
		dropDecoded();
		return;
	}

	if (dataWrite <= dataRead)
		return;
	//SDL_LockMutex(t->mutex_cpp);
	auto data1 = (uint8_t*)data;
	int pos = dataRead % screamSize;
	int rest = screamSize - pos;
	//一次或者两次，保证缓冲区大小足够
	if (len <= rest)
	{
		control->mixAudio(stream, data1 + pos, len, volume);
	}
	else
	{
		control->mixAudio(stream, data1 + pos, rest, volume);
		control->mixAudio(stream + rest, data1, len - rest, volume);
	}
	//auto readp = data1 + pos;
	//int i = t->_map.size();
	while (haveDecoded())
	{
		auto f = getCurrentFrameData();
		if (!f.data || f.time < 0)
		{
			dropDecoded();
			break;
		}
		if (dataRead >= f.info)
		{
			//printf("drop %I64d\n", t->dataRead - f.info);
			dropDecoded();
			if (dataRead == f.info)
			{
				timeShown = f.time;
				ticksShown = control->getTicks();
				break;
			}
		}
		else
		{
			break;
		}
		
	}
	dataRead += len;
	//SDL_UnlockMutex(t->mutex_cpp);
}

BigPotMediaStream::FrameData BigPotAudioStream::convert(void* p /*= nullptr*/)
{
	datalength = BigPotResample::convert(codecCtx, frame, 
		BP_AUDIO_RESAMPLE_FORMAT, freq, channels, resampleBuffer);
	if (useMap())
	{
		//计算写入位置
		//printf("%I64d,%I64d, %d\n", dataWrite, dataRead, _map.size());
		int pos = dataWrite % screamSize;
		int rest = screamSize - pos;
		//够长一次写入，不够长两次写入，不考虑更长情况，如更长是缓冲区不够，效果也不会正常
		if (datalength <= rest)
		{
			memcpy((uint8_t*)data + pos, resampleBuffer, datalength);
		}
		else
		{
			memcpy((uint8_t*)data + pos, resampleBuffer, rest);
			memcpy((uint8_t*)data, resampleBuffer, datalength - rest);
		}
		FrameData f = { timedts, dataWrite, data };
		dataWrite += datalength;
		//返回的是指针位置
		return f;
	}
	else
	{
		memcpy(data, resampleBuffer, datalength);
		return{ timedts, datalength, data };
	}
}

void BigPotAudioStream::freeData(void* p)
{
	//av_free(p);
}

int BigPotAudioStream::setVolume(int v)
{
	v = max(v, 0);
	v = min(v, control->getMaxVolume());
	//printf("\rvolume is %d\t\t\t\t\t", v);
	return volume = v;
}

int BigPotAudioStream::changeVolume(int v)
{
	if (v == 0)
		return volume;
	return setVolume(volume + v);
}

bool BigPotAudioStream::needDecode2()
{
	//return true;
	return dataWrite - dataRead < screamSize / 2;
}

void BigPotAudioStream::resetDecodeState()
{
	dataWrite = dataRead = 0;
	//memset(data, 0, screamSize);
}

bool BigPotAudioStream::setPause(bool pause)
{
	control->pauseAudio(pause);
	return pause;
}


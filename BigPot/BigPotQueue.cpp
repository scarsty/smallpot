#include "BigPotQueue.h"
#include "BigPotResample.h"


BigPotQueue::BigPotQueue()
{
}


BigPotQueue::~BigPotQueue()
{
}

void BigPotQueue::init(BigPotMediaStream* mediaInfo)
{
	w = mediaInfo->codecCtx->width;
	h = mediaInfo->codecCtx->height;
	this->mediaInfo = mediaInfo;
}

//此值为负会在解码时重计算开始时间
void BigPotQueue::resetTimeBegin()
{
	timeBegin = -1;
}

int BigPotVideoQueue::tryDecodeTexture()
{
	if ((time=mediaInfo->readFrame())>=0)
	{
		if (!decoded && mediaInfo->decodeFrame())
		{
			if (timeBegin < 0)
				timeBegin = time;
			//SDL_Texture* tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, w, h);
			auto f = mediaInfo->frame;
			SDL_UpdateYUVTexture(tex, nullptr, f->data[0], f->linesize[0], f->data[1], f->linesize[1], f->data[2], f->linesize[2]);
			//q.push({ time, -1, tex });
			//cout << q.size()<<endl;
			decoded = true;
		}
	}
	return 0;
}

//成功贴图返回0, 没有贴图返回-1
int BigPotVideoQueue::showTexture(int time)
{
	if (decoded && this->time <= time)
	{
		SDL_RenderCopy(ren, tex, nullptr, nullptr);
		//SDL_DestroyTexture(a.tex);
		//q.pop();
		//break;
		decoded = false;
		return 0;
	}
	else
		return -1;
}

int BigPotVideoQueue::dropTexture()
{
	return 0;
}

void BigPotVideoQueue::clear()
{

}

int BigPotAudioQueue::decodeFrame()
{
	if (mediaInfo->readFrame() >= 0&&mediaInfo->decodeFrame())
	{
		int time = mediaInfo->packet->dts * mediaInfo->timePerFrame;
		if (timeBegin < 0)
			timeBegin = time;
		BigPotResample::convert(mediaInfo->codecCtx, mediaInfo->frame, h);
	}
	return 0;
}

int BigPotAudioQueue::getTime()
{
	return timeBegin + BASS_ChannelBytes2Seconds(h, BASS_ChannelGetPosition(h, BASS_POS_BYTE)) * 1e3;

}

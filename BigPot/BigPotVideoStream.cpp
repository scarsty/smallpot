#include "BigPotVideoStream.h"


BigPotVideoStream::BigPotVideoStream()
{
	//视频缓冲区, 足够大时会较流畅，但是跳帧会闪烁
	maxSize = 0;
}


BigPotVideoStream::~BigPotVideoStream()
{
}

int BigPotVideoStream::showTexture(int time)
{
	if (streamIndex < 0)
		return 0;
	auto f = getCurrentFrameData();
	int time_c = f.time;
	if (haveDecoded() && time >= time_c)
	{
		auto tex = (BP_Texture*)f.data;
		control->renderCopy(tex);
		timeShown = time_c;
		ticksShown = control->getTicks();
		dropDecoded();
		return 0;
	}
	return -1;	
}

void BigPotVideoStream::freeData(void* p)
{
	control->destroyTexture((BP_Texture*)p);
}

BigPotMediaStream::FrameData BigPotVideoStream::convert(void* p /*= nullptr*/)
{
	auto &f = frame;
	auto tex = (BP_Texture*)data;
	if (useMap())
	{
		tex = control->createYUVTexture(codecCtx->width, codecCtx->height);
	}
	control->updateYUVTexture(tex, f->data[0], f->linesize[0], f->data[1], f->linesize[1], f->data[2], f->linesize[2]);
	return{ timedts, f->linesize[0], tex };
}

int BigPotVideoStream::dropTexture()
{
	return 0;
}

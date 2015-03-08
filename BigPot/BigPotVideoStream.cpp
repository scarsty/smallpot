#include "BigPotVideoStream.h"


BigPotVideoStream::BigPotVideoStream()
{
	//视频缓冲区, 足够大时会较流畅，但是跳帧会闪烁
	maxSize = 0;
}


BigPotVideoStream::~BigPotVideoStream()
{
}

//-1无视频
//1有可显示的包，未到时间
//2已经没有可显示的包
int BigPotVideoStream::showTexture(int time)
{
	if (streamIndex < 0)
		return -1;
	auto f = getCurrentFrameData();
	int time_c = f.time;
	if (haveDecoded())
	{
		if (time >= time_c)
		{
			auto tex = (BP_Texture*)f.data;
			engine->renderCopy(tex);
			timeShown = time_c;
			ticksShown = engine->getTicks();
			dropDecoded();
			return 0;
		}
		else
		{
			return 1;
		}
	}
	return 2;	
}

void BigPotVideoStream::freeData(void* p)
{
	engine->destroyTexture((BP_Texture*)p);
}

BigPotMediaStream::FrameData BigPotVideoStream::convert(void* p /*= nullptr*/)
{
	auto &f = frame;
	auto tex = (BP_Texture*)data;
	if (useMap())
	{
		tex = engine->createYUVTexture(codecCtx->width, codecCtx->height);
	}
	engine->updateYUVTexture(tex, f->data[0], f->linesize[0], f->data[1], f->linesize[1], f->data[2], f->linesize[2]);
	return{ timedts, f->linesize[0], tex };
}

int BigPotVideoStream::dropTexture()
{
	return 0;
}

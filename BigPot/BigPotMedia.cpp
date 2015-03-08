#include "BigPotMedia.h"

BigPotMedia::BigPotMedia()
{
	_videoStream = new BigPotVideoStream();
	_audioStream = new BigPotAudioStream();
}


BigPotMedia::~BigPotMedia()
{
	delete _videoStream;
	delete _audioStream;
}

int BigPotMedia::openFile(const string &filename)
{
	if (!fileexist(filename))
		return -1;
	_videoStream->openFile(filename, BPMEDIA_TYPE_VIDEO);
	_audioStream->openFile(filename, BPMEDIA_TYPE_AUDIO);

	if (_audioStream->exist())
	{
		_totalTime = _audioStream->getTotalTime();
		_audioStream->openAudioDevice();
	}
	else
		_totalTime = _videoStream->getTotalTime();
	return 0;
}


int BigPotMedia::decodeFrame()
{
	_videoStream->decodeFrame();
	_audioStream->decodeFrame();
	//seek之后，音频可能落后，需要追赶音频
	if (_seeking)
	{
		_seeking = false;
		if (_videoStream->exist() && _audioStream->exist())
		{
			//如某一个延迟则跳过避免迟滞
			_videoStream->skipFrame(_audioStream->getTimedts());
			_audioStream->skipFrame(_videoStream->getTimedts());
		}
		else
		{
			//这里强制显示一帧视频回复视频的时间轴，无音频文件可能需要
			if (_videoStream->exist())
				_videoStream->showTexture(INT32_MAX);
		}
		//printf("\naudio %4.3f, video %4.3f\t\t", audioStream->timed / 1e3, videoStream->timed / 1e3);
	}
	return 0;
}

int BigPotMedia::getAudioTime()
{
	//printf("\t\t\t\t\t\t\r%d,%d,%d", audioStream->time, videoStream->time, audioStream->getAudioTime());
	return _audioStream->getTime();
}

int BigPotMedia::seekTime(int time, int direct /*= 1*/)
{
	time = min(time, _totalTime-100);
	_videoStream->seek(time, direct);
	_audioStream->resetDecodeState();
	_audioStream->seek(time, direct);
	_seeking = true;
	return 0;
}

int BigPotMedia::showVideoFrame(int time)
{
	return _videoStream->showTexture(time);
}

int BigPotMedia::seekPos(double pos)
{
	//printf("\nseek %f pos, %f s\n", pos, pos * totalTime / 1e3);
	return seekTime(pos * _totalTime);
}

int BigPotMedia::getVideoTime()
{
	return _videoStream->getTime();
}

int BigPotMedia::getTime()
{
	if (_audioStream->exist())
		return _audioStream->getTime();
	else
		return _videoStream->getTime();
}

void BigPotMedia::destroy()
{

}


#include "BigPotMedia.h"

BigPotMedia::BigPotMedia()
{
	_videoStream = new BigPotVideoStream();
	_audioStream = new BigPotAudioStream();
	//_subtitle = new BigPotSubtitle();
}


BigPotMedia::~BigPotMedia()
{
	delete _videoStream;
	delete _audioStream;
	//delete _subtitle;
}

int BigPotMedia::openFile(const string &filename)
{
	if (!fileExist(filename))
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

	//int m = _audioStream->getTimedts();
	//int n = _videoStream->getTimedts();

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
	_audioStream->seek(time, direct);
	//_seeking = true;

	//seek之后，音频可能落后，需要追赶音频
	if (time > 0)
	{
		_seeking = false;
		//_videoStream->showTexture(INT32_MAX);
		if (_videoStream->exist() && _audioStream->exist())
		{
			_videoStream->decodeFramePre();
			_audioStream->decodeFramePre();
			//如某一个延迟则跳过避免迟滞
			int max_dts = max(_audioStream->getTimedts(), _videoStream->getTimedts());
			int min_dts = min(_audioStream->getTimedts(), _videoStream->getTimedts());
			if (max_dts - min_dts > 500)
			{
				_videoStream->skipFrame(max_dts);
				_audioStream->skipFrame(max_dts);
			}
			//暂停时要再弃掉一帧数据，原因不明
			if (_audioStream->getPauseState())
				_videoStream->decodeFramePre();
			//_audioStream->decodeFramePre();
		}
		else
		{
			//这里强制显示一帧视频回复视频的时间轴，无音频文件可能需要
			if (_videoStream->exist())
				_videoStream->showTexture(INT32_MAX);
		}
		//_videoStream->decodeFrame();
		//_videoStream->showTexture(INT32_MAX);
		//printf("\naudio %4.3f, video %4.3f\t\t", audioStream->timed / 1e3, videoStream->timed / 1e3);
	}
	_audioStream->resetDecodeState();

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


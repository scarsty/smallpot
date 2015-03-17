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

	//seek之后，音频可能落后，需要追赶音频
	if (time > 0)
	{
		//_videoStream->showTexture(INT32_MAX);
		//预解一阵恢复时间轴，相当于弃掉一帧，基本不会有性能影响
		//通常这一帧也是先前的内容
		//该方法在rm文件会产生马赛克问题，应是解码器关系
		_videoStream->decodeFramePre();
		_audioStream->decodeFramePre();
		//暂停时要弃掉数据到下一个关键帧, 否则画面不会立刻变化
		if (_videoStream->exist() && _videoStream->isPause())
		{			
			//查到关键帧，至少去掉2帧（该值比较奇怪），至多10帧
			int i = 0;
			bool found = false;
			for (i = 0; i < 10; i++)
			{
				_videoStream->decodeFramePre();
				if (!found && _videoStream->isKeyFrame())
				{
					found = true;
				}
				if (i>0 && found)
					break;				
			}
			printf("drop %d frames to find keyframe\n", i + 1);
		}
		//音视频互相同步
		if (_videoStream->exist() && _audioStream->exist())
		{
			//一定时间以上才跳帧
			//查看延迟情况
			int v_dts = _videoStream->getTimedts();
			int a_dts = _audioStream->getTimedts();
			int max_dts = max(v_dts,a_dts);
			int min_dts = min(v_dts, a_dts);
			//printf("seeking diff v%d-a%d=%d\n", v_dts, a_dts, v_dts - a_dts);
			//一定时间以上才跳帧
			if (max_dts - min_dts > 200)
			{
				int sv = _videoStream->skipFrame(max_dts);
				int sa = _audioStream->skipFrame(max_dts);
				printf("drop %d audio frames, %d video frames\n", sa,sv);
			}

			//要弃掉数据到下一个关键帧, 否则画面不会立刻变化（在上面解决）
			/*if (_audioStream->isPause())
			{
				for (int i = 0; i < 10; i++)
				{
					_videoStream->decodeFramePre();
					if (_videoStream->isKeyFrame())
						break;
				}
			}*/
			//_audioStream->decodeFramePre();
		}
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

void BigPotMedia::setPause(bool pause)
{
	_audioStream->setPause(pause);
	_videoStream->setPause(pause);
}


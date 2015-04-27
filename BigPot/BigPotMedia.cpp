#include "BigPotMedia.h"

BigPotMedia::BigPotMedia()
{
	_streamVideo = new BigPotStreamVideo();
	_streamAudio = new BigPotStreamAudio();
	//_subtitle = new BigPotSubtitle();
}


BigPotMedia::~BigPotMedia()
{
	delete _streamVideo;
	delete _streamAudio;
	//delete _subtitle;
}

int BigPotMedia::openFile(const string &filename)
{
	if (!fileExist(filename))
		return -1;
	_streamVideo->openFile(filename, BPMEDIA_TYPE_VIDEO);
	_streamAudio->openFile(filename, BPMEDIA_TYPE_AUDIO);

	if (_streamAudio->exist())
	{
		_totalTime = _streamAudio->getTotalTime();
		_streamAudio->openAudioDevice();
	}
	else
		_totalTime = _streamVideo->getTotalTime();
	return 0;
}


int BigPotMedia::decodeFrame()
{	
	//int se= engine_->getTicks();
	_streamVideo->decodeFrame(_seeking);
	_streamAudio->decodeFrame(_seeking);

	//int m = _audioStream->getTimedts();
	//int n = _videoStream->getTimedts();
	if (_seeking)
	{
		_seeking = false;
		//seek之后，音频可能落后，需要追赶音频
		if (time > 0)
		{
			if (_streamVideo->exist() && _streamAudio->exist())
			{
				//一定时间以上才跳帧
				//查看延迟情况
				int v_dts = _streamVideo->getTimedts();
				int a_dts = _streamAudio->getTimedts();
				int max_dts = max(v_dts, a_dts);
				int min_dts = min(v_dts, a_dts);
				printf("seeking diff v%d-a%d=%d\n", v_dts, a_dts, v_dts - a_dts);
				//一定时间以上才跳帧
				if (max_dts - min_dts > 100)
				{
					int sv = _streamVideo->skipFrame(max_dts);
					int sa = _streamAudio->skipFrame(max_dts);
					printf("drop %d audio frames, %d video frames\n", sa, sv);
					/*v_dts = _videoStream->getTimedts();
					a_dts = _audioStream->getTimedts();
					printf("seeking end diff v%d-a%d=%d\n", v_dts, a_dts, v_dts - a_dts);*/
				}
			}
		}
		//cout << "se"<<engine_->getTicks()-se << " "<<endl;		
	}
	
	return 0;
}

int BigPotMedia::getAudioTime()
{
	//printf("\t\t\t\t\t\t\r%d,%d,%d", audioStream->time, videoStream->time, audioStream->getAudioTime());
	return _streamAudio->getTime();
}

int BigPotMedia::seekTime(int time, int direct /*= 1*/)
{
	time = min(time, _totalTime-100);
	_streamVideo->seek(time, direct);
	_streamAudio->seek(time, direct);

	_seeking = true;
	
	_streamAudio->resetDecodeState();
	
	return 0;
}

int BigPotMedia::showVideoFrame(int time)
{
	return _streamVideo->showTexture(time);
}

int BigPotMedia::seekPos(double pos)
{
	//printf("\nseek %f pos, %f s\n", pos, pos * totalTime / 1e3);
	return seekTime(pos * _totalTime);
}

int BigPotMedia::getVideoTime()
{
	return _streamVideo->getTime();
}

int BigPotMedia::getTime()
{
	if (_streamAudio->exist())
		return _streamAudio->getTime();
	else
		return _streamVideo->getTime();
}

void BigPotMedia::destroy()
{

}

void BigPotMedia::setPause(bool pause)
{
	_streamAudio->setPause(pause);
	_streamVideo->setPause(pause);
}


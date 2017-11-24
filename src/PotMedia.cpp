#include "PotMedia.h"
#include "Config.h"
#include "File.h"

PotMedia::PotMedia()
{
    _streamVideo = new PotStreamVideo();
    _streamAudio = new PotStreamAudio();
    //_streamSubtitle = new BigPotStreamSubtitle();
}


PotMedia::~PotMedia()
{
    delete _streamVideo;
    delete _streamAudio;
    //delete _streamSubtitle;
}

int PotMedia::openFile(const std::string& filename)
{
    if (!File::fileExist(filename))
    {
        return -1;
    }
    _streamVideo->openFile(filename);
    _streamAudio->openFile(filename);
    //_streamSubtitle->openFile(filename);

    if (_streamAudio->exist())
    {
        _totalTime = _streamAudio->getTotalTime();
        _streamAudio->openAudioDevice();
    }
    else
    {
        _totalTime = _streamVideo->getTotalTime();
    }
    return 0;
}


int PotMedia::decodeFrame()
{
    //int se= engine_->getTicks();
    _streamVideo->tryDecodeFrame(_seeking);
    if (_streamVideo->isStopping()) { return 0; }
    //_streamAudio->tryDecodeFrame(_seeking);
    for (int i = 0; i <= _extAudioFrame; i++)
    {
        _streamAudio->tryDecodeFrame(_seeking);
    }
    _streamSubtitle->tryDecodeFrame(_seeking);
    //int m = _audioStream->getTimedts();
    //int n = _videoStream->getTimedts();
    if (_seeking)
    {
        _seeking = false;
        //seek之后，音频可能落后，需要追赶音频
        if (_streamVideo->exist() && _streamAudio->exist())
        {
            //一定时间以上才跳帧
            //查看延迟情况
            int v_dts = _streamVideo->getTimedts();
            int a_dts = _streamAudio->getTimedts();
            int max_dts = std::max(v_dts, a_dts);
            int min_dts = std::min(v_dts, a_dts);
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
        //cout << "se"<<engine_->getTicks()-se << " "<<endl;
    }

    return 0;
}

int PotMedia::getAudioTime()
{
    //printf("\t\t\t\t\t\t\r%d,%d,%d", audioStream->time, videoStream->time, audioStream->getAudioTime());
    return _streamAudio->getTime();
}

int PotMedia::seekTime(int time, int direct /*= 1*/, int reset /*= 0*/)
{
    time = std::min(time, _totalTime - 100);
    _streamVideo->seek(time, direct, reset);
    _streamAudio->seek(time, direct, reset);

    _seeking = true;

    _streamAudio->resetDecodeState();

    return 0;
}

int PotMedia::showVideoFrame(int time)
{
    return _streamVideo->showTexture(time);
}

int PotMedia::seekPos(double pos, int direct /*= 1*/, int reset /*= 0*/)
{
    //printf("\nseek %f pos, %f s\n", pos, pos * totalTime / 1e3);
    return seekTime(pos * _totalTime, direct, reset);
}

int PotMedia::getVideoTime()
{
    return _streamVideo->getTime();
}

int PotMedia::getTime()
{
    if (_streamAudio->exist())
    {
        return _streamAudio->getTime();
    }
    else
    {
        return _streamVideo->getTime();
    }
}

void PotMedia::destroy()
{

}

bool PotMedia::isMedia()
{
    return _streamAudio->exist() || _streamVideo->exist();
}

void PotMedia::setPause(bool pause)
{
    _streamAudio->setPause(pause);
    _streamVideo->setPause(pause);
}




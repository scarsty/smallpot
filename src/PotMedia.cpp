#include "PotMedia.h"
#include "Config.h"
#include "File.h"

PotMedia::PotMedia()
{
    stream_video_ = new PotStreamVideo();
    stream_audio_ = new PotStreamAudio();
    //_streamSubtitle = new BigPotStreamSubtitle();
}


PotMedia::~PotMedia()
{
    delete stream_video_;
    delete stream_audio_;
    //delete _streamSubtitle;
}

int PotMedia::openFile(const std::string& filename)
{
    if (!File::fileExist(filename))
    {
        return -1;
    }
    stream_video_->openFile(filename);
    stream_audio_->openFile(filename);
    //_streamSubtitle->openFile(filename);

    if (stream_audio_->exist())
    {
        total_time_ = stream_audio_->getTotalTime();
        stream_audio_->openAudioDevice();
    }
    else
    {
        total_time_ = stream_video_->getTotalTime();
    }
    return 0;
}


int PotMedia::decodeFrame()
{
    //int se= engine_->getTicks();
    stream_video_->tryDecodeFrame(seeking_);
    if (stream_video_->isStopping()) { return 0; }
    //_streamAudio->tryDecodeFrame(_seeking);
    for (int i = 0; i <= ext_audio_frame_; i++)
    {
        stream_audio_->tryDecodeFrame(seeking_);
    }
    stream_subtitle_->tryDecodeFrame(seeking_);
    //int m = _audioStream->getTimedts();
    //int n = _videoStream->getTimedts();
    if (seeking_)
    {
        seeking_ = false;
        //seek之后，音频可能落后，需要追赶音频
        if (stream_video_->exist() && stream_audio_->exist())
        {
            //一定时间以上才跳帧
            //查看延迟情况
            int v_dts = stream_video_->getTimedts();
            int a_dts = stream_audio_->getTimedts();
            int max_dts = std::max(v_dts, a_dts);
            int min_dts = std::min(v_dts, a_dts);
            printf("seeking diff v%d-a%d=%d\n", v_dts, a_dts, v_dts - a_dts);
            //一定时间以上才跳帧
            if (max_dts - min_dts > 100)
            {
                int sv = stream_video_->skipFrame(max_dts);
                int sa = stream_audio_->skipFrame(max_dts);
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
    return stream_audio_->getTime();
}

int PotMedia::seekTime(int time, int direct /*= 1*/, int reset /*= 0*/)
{
    time = std::min(time, total_time_ - 100);
    stream_video_->seek(time, direct, reset);
    stream_audio_->seek(time, direct, reset);

    seeking_ = true;

    stream_audio_->resetDecodeState();

    return 0;
}

int PotMedia::showVideoFrame(int time)
{
    return stream_video_->showTexture(time);
}

int PotMedia::seekPos(double pos, int direct /*= 1*/, int reset /*= 0*/)
{
    //printf("\nseek %f pos, %f s\n", pos, pos * totalTime / 1e3);
    return seekTime(pos * total_time_, direct, reset);
}

int PotMedia::getVideoTime()
{
    return stream_video_->getTime();
}

int PotMedia::getTime()
{
    if (stream_audio_->exist())
    {
        return stream_audio_->getTime();
    }
    else
    {
        return stream_video_->getTime();
    }
}

void PotMedia::destroy()
{

}

bool PotMedia::isMedia()
{
    return stream_audio_->exist() || stream_video_->exist();
}

void PotMedia::setPause(bool pause)
{
    stream_audio_->setPause(pause);
    stream_video_->setPause(pause);
}




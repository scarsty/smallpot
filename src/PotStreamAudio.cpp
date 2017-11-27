#include "PotStreamAudio.h"

PotStreamAudio::PotStreamAudio()
{
    volume_ = engine_->getMaxVolume() / 2;
    //预解包数量
    //除非知道音频包定长，否则不应设为0，一般情况下都不建议为0
    max_size_ = 100;
    //缓冲区大小4M保存
    if (useMap())
    {
        data_ = av_mallocz(scream_size_);
    }
    resample_buffer_ = (decltype(resample_buffer_))av_mallocz(convert_size_);
    type_ = BPMEDIA_TYPE_AUDIO;
}


PotStreamAudio::~PotStreamAudio()
{
    if (useMap())
    {
        av_free(data_);
    }
    if (resample_buffer_)
    {
        av_free(resample_buffer_);
    }
    engine_->setAudioCallback(nullptr);
    closeAudioDevice();
}

void PotStreamAudio::openAudioDevice()
{
    if (stream_index_ < 0)
    {
        return;
    }
    freq_ = codec_ctx_->sample_rate;
    channels_ = config_->getInteger("channels", -1);
    if (channels_ < 0)
    {
        channels_ = codec_ctx_->channels;
    }
    engine_->openAudio(freq_, channels_, codec_ctx_->frame_size, 2048, std::bind(&PotStreamAudio::mixAudioData, this, std::placeholders::_1, std::placeholders::_2));

    auto audio_format = AV_SAMPLE_FMT_S16;

    std::map<SDL_AudioFormat, AVSampleFormat> SDL_FFMPEG_AUDIO_FORMAT =
    {
        { AUDIO_U8, AV_SAMPLE_FMT_U8 },
        { AUDIO_S16, AV_SAMPLE_FMT_S16 },
        { AUDIO_S32, AV_SAMPLE_FMT_S32 },
        { AUDIO_F32, AV_SAMPLE_FMT_FLT },
        //{ AUDIO_U8, AV_SAMPLE_FMT_DBL },
    };

    resample_.setOutFormat(SDL_FFMPEG_AUDIO_FORMAT[engine_->getAudioFormat()]);
}

int PotStreamAudio::closeAudioDevice()
{
    engine_->closeAudio();
    return 0;
}

void PotStreamAudio::mixAudioData(uint8_t* stream, int len)
{
    if (!useMap())
    {
        engine_->mixAudio(stream, (uint8_t*)resample_buffer_, len, volume_);
        dropDecoded();
        return;
    }

    if (data_write_ <= data_read_)
    {
        return;
    }
    //SDL_LockMutex(t->mutex_cpp);
    auto data1 = (uint8_t*)data_;
    int pos = data_read_ % scream_size_;
    int rest = scream_size_ - pos;
    //一次或者两次，保证缓冲区大小足够
    if (len <= rest)
    {
        engine_->mixAudio(stream, data1 + pos, len, volume_);
    }
    else
    {
        engine_->mixAudio(stream, data1 + pos, rest, volume_);
        engine_->mixAudio(stream + rest, data1, len - rest, volume_);
    }
    //auto readp = data1 + pos;
    //int i = t->_map.size();
    while (haveDecoded())
    {
        auto f = getCurrentContent();
        if (!f.data || f.time < 0)
        {
            dropDecoded();
            break;
        }
        if (data_read_ >= f.info)
        {
            //printf("drop %I64d\n", t->dataRead - f.info);
            dropDecoded();
            if (data_read_ == f.info && time_shown_ != f.time)
            {
                time_shown_ = f.time;
                ticks_shown_ = engine_->getTicks();
                break;
            }
            else
            {
                //获取下一个
                auto f1 = getCurrentContent();
                //后一个包如果时间是一样的不更新计时
                if (f1.info > data_read_ && f.time != f1.time)
                {
                    time_shown_ = f.time + (f1.time - f.time) * (data_read_ - f.info) / (f1.info - f.info);
                    ticks_shown_ = engine_->getTicks();
                }
            }
        }
        else
        {
            break;
        }
    }
    data_read_ += len;
    //cout<<time_shown_<<endl;
    //SDL_UnlockMutex(t->mutex_cpp);
}

PotStream::Content PotStreamAudio::convertFrameToContent(void* p /*= nullptr*/)
{
    data_length_ = resample_.convert(codec_ctx_, frame_, freq_, channels_, resample_buffer_);
    if (data_length_ <= 0)
    {
        return{ -1, data_length_, nullptr };
    }
    if (useMap())
    {
        //计算写入位置
        //printf("%I64d,%I64d, %d\n", dataWrite, dataRead, _map.size());
        int pos = data_write_ % scream_size_;
        int rest = scream_size_ - pos;
        //够长一次写入，不够长两次写入，不考虑更长情况，如更长是缓冲区不够，效果也不会正常
        if (data_length_ <= rest)
        {
            memcpy((uint8_t*)data_ + pos, resample_buffer_, data_length_);
        }
        else
        {
            memcpy((uint8_t*)data_ + pos, resample_buffer_, rest);
            memcpy((uint8_t*)data_, resample_buffer_, data_length_ - rest);
        }
        Content f = { time_dts_, data_write_, data_ };
        data_write_ += data_length_;
        //返回的是指针位置
        return f;
    }
    else
    {
        memcpy(data_, resample_buffer_, data_length_);
        return{ time_dts_, data_length_, data_ };
    }
}

void PotStreamAudio::freeContent(void* p)
{
    //av_free(p);
}

int PotStreamAudio::setVolume(int v)
{
    v = std::max(v, 0);
    v = std::min(v, engine_->getMaxVolume());
    //printf("\rvolume is %d\t\t\t\t\t", v);
    return volume_ = v;
}

int PotStreamAudio::changeVolume(int v)
{
    if (v == 0)
    {
        return volume_;
    }
    return setVolume(volume_ + v);
}

bool PotStreamAudio::needDecode2()
{
    //return true;
    return data_write_ - data_read_ < scream_size_ / 2;
}

void PotStreamAudio::resetDecodeState()
{
    data_write_ = data_read_ = 0;
    //memset(data, 0, screamSize);
}

void PotStreamAudio::setPause(bool pause)
{
    engine_->pauseAudio(pause);
    pause_ = pause;
    pause_time_ = getTime();
    ticks_shown_ = engine_->getTicks();
}



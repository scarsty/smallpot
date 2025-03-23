#include "PotStreamAudio.h"
#include "Config.h"

float PotStreamAudio::volume_;

PotStreamAudio::PotStreamAudio()
{
    //volume_ = engine_->getMaxVolume() / 2;
    //预解包数量
    //除非知道音频包定长，否则不应设为0，一般情况下都不建议为0
    max_size_ = 100;
    //缓冲区大小4M保存
    buffer_ = av_mallocz(buffer_size_);
    resample_buffer_ = (decltype(resample_buffer_))av_mallocz(convert_size_);
    type_ = MEDIA_TYPE_AUDIO;
    decode_frame_count_ = 2;
}

PotStreamAudio::~PotStreamAudio()
{
    av_free(buffer_);
    buffer_ = nullptr;
    if (resample_buffer_)
    {
        av_free(resample_buffer_);
        resample_buffer_ = nullptr;
    }
    //engine_->setAudioCallback(nullptr);
    closeAudioDevice();
}

void PotStreamAudio::mixAudioData(uint8_t* stream, int len)
{
    if (buffer_ == nullptr) { return; }
    if (data_write_ <= data_read_)
    {
        return;
    }
    //SDL_LockMutex(t->mutex_cpp);
    auto data1 = (uint8_t*)buffer_;
    int pos = data_read_ % buffer_size_;
    int rest = buffer_size_ - pos;
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
            //std::print("drop %I64d\n", t->dataRead - f.info);
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

FrameContent PotStreamAudio::convertFrameToContent()
{
    bool isplanar = av_sample_fmt_is_planar(codec_ctx_->sample_fmt) == 1;
    //sdl不支持planar格式
    //data_length_ = resample_.convert(codec_ctx_, frame_, freq_, channels_, resample_buffer_);
    if (isplanar)
    {
        //int size;
        //int bufsize = av_samples_get_buffer_size(&size, codec_ctx_->ch_layout.nb_channels, frame_->nb_samples, codec_ctx_->sample_fmt, 0);
        //memcpy(resample_buffer_, frame_->data[0] + size * codec_ctx_->ch_layout.nb_channels, size);
        //data_length_ = size;
        data_length_ = resample_.convert(codec_ctx_, frame_, freq_, channels_, resample_buffer_);
    }
    else
    {
        memcpy(resample_buffer_, frame_->data[0], frame_->linesize[0]);
        data_length_ = frame_->linesize[0];
    }
    if (data_length_ <= 0)
    {
        return { -1, data_length_, nullptr };
    }
    //计算写入位置
    //std::print("%I64d,%I64d, %d\n", dataWrite, dataRead, _map.size());
    int pos = data_write_ % buffer_size_;
    int rest = buffer_size_ - pos;
    //够长一次写入，不够长两次写入，不考虑更长情况，如更长是缓冲区不够，效果也不会正常
    if (data_length_ <= rest)
    {
        memcpy((uint8_t*)buffer_ + pos, resample_buffer_, data_length_);
    }
    else
    {
        memcpy((uint8_t*)buffer_ + pos, resample_buffer_, rest);
        memcpy((uint8_t*)buffer_, resample_buffer_, data_length_ - rest);
    }
    FrameContent f = { time_dts_, data_write_, buffer_ };
    data_write_ += data_length_;
    //返回的是指针位置
    return f;
}

int PotStreamAudio::show()
{
    if (data_map_.size() >= max_size_) { return 1; }
    const int len = 3840;
    if (buffer_ == nullptr) { return -1; }
    if (data_write_ <= data_read_)
    {
        return -2;
    }
    //SDL_LockMutex(t->mutex_cpp);
    auto data1 = (uint8_t*)buffer_;
    int pos = data_read_ % buffer_size_;
    int rest = buffer_size_ - pos;
    //一次或者两次，保证缓冲区大小足够
    if (len <= rest)
    {
        engine_->putAudioStreamData(data1 + pos, len);
    }
    else
    {
        engine_->putAudioStreamData(data1 + pos, rest);
        engine_->putAudioStreamData(data1, len - rest);
    }
    while (haveDecoded())
    {
        auto f = getCurrentContent();
        Engine::getInstance()->putAudioStreamData(resample_buffer_, data_length_);
        if (!f.data || f.time < 0)
        {
            dropDecoded();
            break;
        }
        if (data_read_ >= f.info)
        {
            //std::print("drop %I64d\n", t->dataRead - f.info);
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
    return 0;
}

bool PotStreamAudio::needDecode2()
{
    if (buffer_ == nullptr) { return false; }
    return (data_write_ - data_read_ < buffer_size_ / 2);
}

void PotStreamAudio::openAudioDevice()
{
    if (stream_index_ < 0)
    {
        return;
    }
    freq_ = codec_ctx_->sample_rate;
    channels_ = Config::getInstance().get("channels", -1);
    if (channels_ < 0)
    {
        channels_ = codec_ctx_->ch_layout.nb_channels;
    }
    //注意这里仍然使用的是回调，计算时间比较简单
    engine_->openAudio(freq_, channels_, codec_ctx_->frame_size, 2048, [this](uint8_t* stream, int len)
        {
            mixAudioData(stream, len);
        });

    auto audio_format = AV_SAMPLE_FMT_S16;

    std::map<SDL_AudioFormat, AVSampleFormat> SDL_FFMPEG_AUDIO_FORMAT = {
        { SDL_AUDIO_U8, AV_SAMPLE_FMT_U8 },
        { SDL_AUDIO_S16LE, AV_SAMPLE_FMT_S16 },
        { SDL_AUDIO_S32LE, AV_SAMPLE_FMT_S32 },
        { SDL_AUDIO_F32LE, AV_SAMPLE_FMT_FLT },
        //{ AUDIO_U8, AV_SAMPLE_FMT_DBL },
    };

    resample_.setOutFormat(SDL_FFMPEG_AUDIO_FORMAT[engine_->getAudioFormat()]);
}

int PotStreamAudio::closeAudioDevice()
{
    engine_->closeAudio();
    return 0;
}

void PotStreamAudio::resetDecodeState()
{
    data_write_ = data_read_ = 0;
    //memset(data, 0, screamSize);
}

void PotStreamAudio::setVolume(float v)
{
    v = std::max(v, 0.0f);
    v = std::min(v, Engine::getMaxVolume());
    //std::print("\rvolume is %d\t\t\t\t\t", v);
    volume_ = v;
    Engine::getInstance()->setAudioStreamGain(v);
}

void PotStreamAudio::changeVolume(float v)
{
    if (v != 0)
    {
        setVolume(volume_ + v);
    }
}

void PotStreamAudio::setPause(bool pause)
{
    engine_->pauseAudio(pause);
    pause_ = pause;
    pause_time_ = getTime();
    ticks_shown_ = engine_->getTicks();
}

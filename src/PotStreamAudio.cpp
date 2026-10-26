#include "PotStreamAudio.h"
#ifndef _WINDLL
#include "Config.h"
#endif

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

FrameContent PotStreamAudio::convertFrameToContent()
{
    const bool needs_conversion = av_sample_fmt_is_planar(codec_ctx_->sample_fmt)
        || codec_ctx_->sample_fmt != resample_.out_sample_format_
        || codec_ctx_->sample_rate != freq_
        || codec_ctx_->ch_layout.nb_channels != channels_;

    if (needs_conversion)
    {
        data_length_ = resample_.convert(codec_ctx_.get(), frame_.get(), freq_, channels_, resample_buffer_);
    }
    else
    {
        int linesize = 0;
        data_length_ = av_samples_get_buffer_size(
            &linesize,
            codec_ctx_->ch_layout.nb_channels,
            frame_->nb_samples,
            codec_ctx_->sample_fmt,
            1);
        if (data_length_ > 0)
        {
            memcpy(resample_buffer_, frame_->data[0], data_length_);
        }
    }
    if (data_length_ <= 0)
    {
        return FrameContent{ -1, data_length_, std::monostate{} };
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
    FrameContent f{ time_dts_, data_write_, static_cast<uint8_t*>(buffer_) };
    data_write_ += data_length_;
    //返回的是指针位置
    return f;
}

int PotStreamAudio::show()
{
    if (pause_ || !exist())
    {
        return 0;
    }
    const int bytes_per_frame = channels_ * av_get_bytes_per_sample(static_cast<AVSampleFormat>(resample_.out_sample_format_));
    const int64_t bytes_per_second = int64_t(freq_) * bytes_per_frame;
    if (!buffer_ || bytes_per_frame <= 0 || bytes_per_second <= 0)
    {
        return -1;
    }
    const int queued = engine_->getQueuedAudioBytes();
    if (queued < 0)
    {
        return -1;
    }
    // Submission is not playback: retain timestamp markers until SDL consumes the data.
    const int64_t played = std::max<int64_t>(0, data_read_ - queued);
    while (data_map_.size() > 1)
    {
        auto next = std::next(data_map_.begin());
        if (next->second.info > played)
        {
            break;
        }
        dropDecoded();
    }
    if (haveDecoded())
    {
        const auto f = getCurrentContent();
        if (std::holds_alternative<uint8_t*>(f.data) && f.time >= 0 && played >= f.info)
        {
            resetTimeAxis(f.time + int((played - f.info) * 1000 / bytes_per_second));
        }
    }
    // Keep at most 80 ms queued, independent of the UI loop or display refresh rate.
    const int64_t target = std::max<int64_t>(bytes_per_frame, bytes_per_second * 80 / 1000);
    int64_t len = std::min<int64_t>(data_write_ - data_read_, std::max<int64_t>(0, target - queued));
    len -= len % bytes_per_frame;
    if (len <= 0)
    {
        return 0;
    }
    const int pos = int(data_read_ % buffer_size_);
    const int first = int(std::min<int64_t>(len, buffer_size_ - pos));
    engine_->putAudioStreamData(static_cast<uint8_t*>(buffer_) + pos, first);
    if (len > first)
    {
        engine_->putAudioStreamData(buffer_, int(len - first));
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
#ifndef _WINDLL
    channels_ = Config::getInstance().get("channels", -1);
#endif
    if (channels_ < 0)
    {
        channels_ = codec_ctx_->ch_layout.nb_channels;
    }
    auto input_format = SDL_AUDIO_F32LE;
    switch (codec_ctx_->sample_fmt)
    {
    case AV_SAMPLE_FMT_U8:
    case AV_SAMPLE_FMT_U8P:
        input_format = SDL_AUDIO_U8;
        break;
    case AV_SAMPLE_FMT_S16:
    case AV_SAMPLE_FMT_S16P:
        input_format = SDL_AUDIO_S16LE;
        break;
    case AV_SAMPLE_FMT_S32:
    case AV_SAMPLE_FMT_S32P:
        input_format = SDL_AUDIO_S32LE;
        break;
    case AV_SAMPLE_FMT_FLT:
    case AV_SAMPLE_FMT_FLTP:
        input_format = SDL_AUDIO_F32LE;
        break;
    default:
        input_format = SDL_AUDIO_F32LE;
        break;
    }

    // SDL3 converts the queued stream format to the physical device format.
    // 单线程模式下由主循环主动提交音频，避免 SDL 回调线程访问解码缓冲区。
    engine_->openAudio(freq_, channels_, codec_ctx_->frame_size, 2048, input_format, nullptr);

    resample_.setOutFormat(input_format == SDL_AUDIO_U8 ? AV_SAMPLE_FMT_U8
        : input_format == SDL_AUDIO_S16LE ? AV_SAMPLE_FMT_S16
        : input_format == SDL_AUDIO_S32LE ? AV_SAMPLE_FMT_S32
        : AV_SAMPLE_FMT_FLT);
}

int PotStreamAudio::closeAudioDevice()
{
    engine_->closeAudio();
    return 0;
}

void PotStreamAudio::resetDecodeState()
{
    data_write_ = data_read_ = 0;
    dropAllDecoded();
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
    PotStream::setPause(pause);
    if (exist())
    {
        engine_->pauseAudio(pause);
    }
}

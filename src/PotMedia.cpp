#include "PotMedia.h"
#include "Config.h"
#include "PotConv.h"
#include "filefunc.h"

PotMedia::PotMedia()
{
    avformat_network_init();
    stream_video_ = &blank_video_;
    stream_audio_ = &blank_audio_;
#ifndef WITHOUT_SUBTITLE
    stream_subtitle_ = &blank_subtitle_;
#endif
    format_ctx_.reset(avformat_alloc_context());
}

PotMedia::~PotMedia()
{
    if (stream_audio_->exist())
    {
        stream_audio_->closeAudioDevice();
    }
    for (auto st : streams_)
    {
        delete st;
    }
    for (auto& packet : packet_queue_)
    {
        av_packet_unref(&packet);
    }
    packet_queue_.clear();
}

int PotMedia::decodeFrame()
{
    bool need_reset = seeking_;
    //int se= engine_->getTicks();
    stream_video_->tryDecodeFrame(need_reset);
    if (stream_video_->isStopping())
    {
        return 0;
    }
    stream_audio_->tryDecodeFrame(need_reset);
#ifndef WITHOUT_SUBTITLE
    stream_subtitle_->tryDecodeFrame(need_reset);
#endif
    //int m = _audioStream->getTimedts();
    //int n = _videoStream->getTimedts();

    // 两路都从同一个 seek 点重新解码，不再用 skipFrame 让视频或音频追赶另一条路。
    seeking_ = false;
    return 0;
}

int PotMedia::readPacket(int stream_index, AVPacket* packet)
{
    while (true)
    {
        for (auto it = packet_queue_.begin(); it != packet_queue_.end(); ++it)
        {
            if (it->stream_index == stream_index)
            {
                av_packet_move_ref(packet, &*it);
                packet_queue_.erase(it);
                return 0;
            }
        }
        if (!format_ctx_ || av_read_frame(format_ctx_.get(), packet) < 0)
        {
            return AVERROR_EOF;
        }
        if (packet->stream_index == stream_index)
        {
            return 0;
        }
        AVPacket queued_packet{};
        av_packet_move_ref(&queued_packet, packet);
        packet_queue_.push_back(queued_packet);
    }
}

int PotMedia::seekDemux(int time, int direct, int reset)
{
    if (!format_ctx_ || seek_stream_index_ < 0 || seek_stream_index_ >= static_cast<int>(format_ctx_->nb_streams))
    {
        return AVERROR(EINVAL);
    }
    for (auto& packet : packet_queue_)
    {
        av_packet_unref(&packet);
    }
    packet_queue_.clear();
    const int flags = direct < 0 ? AVSEEK_FLAG_BACKWARD : 0;
    const AVRational milliseconds = { 1, 1000 };
    const AVRational stream_time_base = format_ctx_->streams[seek_stream_index_]->time_base;
    const int64_t timestamp = av_rescale_q(time, milliseconds, stream_time_base);
    return av_seek_frame(format_ctx_.get(), seek_stream_index_, timestamp, flags);
}

int PotMedia::openFile(const std::string& filename)
{
    AVFormatContext* raw_format_ctx = avformat_alloc_context();
    if (!raw_format_ctx || avformat_open_input(&raw_format_ctx, filename.c_str(), nullptr, nullptr) < 0
        || avformat_find_stream_info(raw_format_ctx, nullptr) < 0)
    {
        avformat_close_input(&raw_format_ctx);
        return -1;
    }
    format_ctx_.reset(raw_format_ctx);

    streams_.resize(format_ctx_->nb_streams);
    for (unsigned int i = 0; i < format_ctx_->nb_streams; i++)
    {
        switch (format_ctx_->streams[i]->codecpar->codec_type)
        {
        case MEDIA_TYPE_VIDEO:
        {
            auto st = new PotStreamVideo();
            streams_[i] = st;
            if (!stream_video_->exist()) stream_video_ = st;
            break;
        }
        case MEDIA_TYPE_AUDIO:
        {
            auto st = new PotStreamAudio();
            streams_[i] = st;
            if (!stream_audio_->exist()) stream_audio_ = st;
            break;
        }
        case MEDIA_TYPE_SUBTITLE:
        {
#ifndef WITHOUT_SUBTITLE
            auto st = new PotStreamSubtitle();
            streams_[i] = st;
            if (!stream_subtitle_->exist()) stream_subtitle_ = st;
#endif
            break;
        }
        default:
            break;
        }
        if (streams_[i])
        {
            streams_[i]->setFormatCtx(format_ctx_.get());
            streams_[i]->setPacketReader([this](int stream_index, AVPacket* packet) { return readPacket(stream_index, packet); });
            streams_[i]->setSeekHandler([this](int time, int direct, int reset) { return seekDemux(time, direct, reset); });
            streams_[i]->setStreamIndex(static_cast<int>(i));
            streams_[i]->openFile(filename);
        }
    }

    seek_stream_index_ = stream_video_->exist() ? stream_video_->getStreamIndex() : stream_audio_->getStreamIndex();
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
int PotMedia::getAudioTime()
{
    //std::print("\t\t\t\t\t\t\r%d,%d,%d", audioStream->time, videoStream->time, audioStream->getAudioTime());
    return stream_audio_->getTime();
}

int PotMedia::getVideoTime()
{
    return stream_video_->getTime();
}

int PotMedia::seekTime(int time, int direct /*= 1*/, int reset /*= 0*/)
{
    time = std::max(0, std::min(time, total_time_ - 100));
    const int seek_time = time;
    if (format_ctx_ && seekDemux(seek_time, direct, reset) < 0)
    {
        return -1;
    }
    stream_video_->resetAfterSeek(seek_time, reset);
    stream_audio_->resetAfterSeek(seek_time, reset);
#ifndef WITHOUT_SUBTITLE
    stream_subtitle_->resetAfterSeek(std::max(0, seek_time - 5000), reset);
#endif
    stream_audio_->resetDecodeState();
    Engine::getInstance()->clearAudioStream();
    stream_video_->setPause(stream_audio_->isPause());
    seeking_ = true;
    return 0;
}

int PotMedia::seekPos(double pos, int direct /*= 1*/, int reset /*= 0*/)
{
    //std::print("\nseek %f pos, %f s\n", pos, pos * totalTime / 1e3);
    return seekTime(pos * total_time_, direct, reset);
}

int PotMedia::showVideoFrame(int time)
{
    return stream_video_->show(time);
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
    if (streams_.empty())
    {
        return false;
    }
    return stream_audio_->exist() || stream_video_->exist();
}

void PotMedia::setPause(bool pause)
{
    stream_audio_->setPause(pause);
    stream_video_->setPause(pause);
    //stream_subtitle_->setPause(pause);
}

void PotMedia::switchStream(PotMediaType mt)
{
    if (getStreamCount(mt) <= 1)
    {
        return;
    }
    int current_index = -1;
    for (int i = 0; i < streams_.size(); i++)
    {
        if (streams_[i] && streams_[i]->getType() == mt
            && (streams_[i] == stream_video_ || streams_[i] == stream_audio_
#ifndef WITHOUT_SUBTITLE
                || streams_[i] == stream_subtitle_
#endif
                ))
        {
            current_index = i;
            break;
        }
    }
    if (current_index == -1)
    {
        return;
    }
    PotStream* st = streams_[current_index];
    for (int i = current_index + 1; i < streams_.size(); i++)
    {
        if (streams_[i] && streams_[i]->getType() == mt)
        {
            st = streams_[i];
            break;
        }
    }
    if (st == streams_[current_index])
    {
        for (int i = 0; i < current_index; i++)
        {
            if (streams_[i] && streams_[i]->getType() == mt)
            {
                st = streams_[i];
                break;
            }
        }
    }

    switch (mt)
    {
    case MEDIA_TYPE_VIDEO:
    {
        stream_video_ = (PotStreamVideo*)st;
        break;
    }
    case MEDIA_TYPE_AUDIO:
    {
        //注意此处效果不佳
        //若假设所有音频使用同一解码器，则切换的效果会较好
        int t = getTime();
        stream_audio_->closeAudioDevice();
        //stream_audio_->clearMap();
        stream_audio_ = (PotStreamAudio*)st;
        stream_audio_->openAudioDevice();
        stream_audio_->seek(t);
        //stream_audio_->setStreamIndex(st->getStreamIndex());
        break;
    }
    case MEDIA_TYPE_SUBTITLE:
    {
#ifndef WITHOUT_SUBTITLE
        stream_subtitle_ = (PotStreamSubtitle*)st;
        stream_subtitle_->seek(getTime() - 5000);
#endif
        break;
    }
    }
}

int PotMedia::getStreamCount(PotMediaType mt)
{
    int count = 0;
    for (int i = 0; i < streams_.size(); i++)
    {
        if (streams_[i] && streams_[i]->getType() == mt && streams_[i]->exist())
        {
            count++;
        }
    }
    return count;
}


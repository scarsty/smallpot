#include "PotStream.h"
#include "PotResample.h"

PotStream::PotStream()
{
    //format_ctx_ = avformat_alloc_context();
    frame_.reset(av_frame_alloc());
    //subtitle_ = av_
    //mutex_cpp.;
    time_shown_ = 0;
    ticks_shown_ = engine_->getTicks();
    av_packet_unref(&packet_);
}

PotStream::~PotStream()
{
    //avformat_close_input(&format_ctx_);
    clearMap();
    stream_index_ = -1;
    //DestroyMutex(mutex_cpp);
}

int PotStream::avcodec_decode_packet(AVCodecContext* ctx, int* n, AVPacket* packet)
{
    int ret;
    *n = 0;
    //if (packet)
    {
        ret = avcodec_send_packet(ctx, packet);
        //std::print("send packet {}\n", ret);
        if (ret < 0)
        {
            if (ret == AVERROR(EAGAIN))
            {
            }
            else if (ret == AVERROR_EOF)
            {
                return ret;
            }
            else if (ret == AVERROR(EINVAL))
            {
                avcodec_flush_buffers(ctx);
                return ret;
            }
            else
            {
                return ret;
            }
        }
    }

    ret = avcodec_receive_frame(ctx, frame_.get());
    //std::print("receive frame {}\n", ret);
    if (ret < 0)
    {
        //std::print(stderr, "Error while decoding frame {}\n", ret);
        return ret;
    }
    if (ret >= 0)
    {
        *n = 1;
    }
    return 0;
}

int PotStream::dropContent()
{
    if (!data_map_.empty())
    {
        freeContent(data_map_.begin()->second);
        data_map_.erase(data_map_.begin());
    }
    return 0;
}

void PotStream::setMap(int key, FrameContent f)
{
    data_map_[key] = f;
}

void PotStream::clearMap()
{
    for (auto& i : data_map_)
    {
        freeContent(i.second);
    }
    data_map_.clear();
}

bool PotStream::needDecode()
{
    if (!needDecode2())
    {
        return false;
    }
    return (data_map_.size() < max_size_);
}

//解压帧，同时会更新当前的时间戳
//在长时间无包时可能造成界面卡死，设法改一下
//ret
//1  解到frame
//2  命令中指定有packet但是没解
//-1 文件损坏或者结束
//-2 命令中指定不是本类型的流也不再继续
//-3 解码错误
int PotStream::decodeNextPacketToFrame(bool decode, bool til_got)
{
    if (!exist())
    {
        return -2;
    }
    int ret = 0;
    int gotframe = 0;
    int decoded_ret = 0;
    bool haveFrame = !need_read_packet_;
    while (ret == 0 || decoded_ret == AVERROR(EAGAIN))
    {
        if (need_read_packet_)
        {
            haveFrame = (read_packet_ ? read_packet_(stream_index_, &packet_) : av_read_frame(format_ctx_, &packet_)) == 0;
        }
        if (haveFrame)
        {
            //只处理本流的数据
            if (packet_.stream_index == stream_index_)
            {
                if (decode)
                {
                    //循环处理多次才能解到一帧的情况
                    //while (gotframe == 0)
                    {
                        decoded_ret = avcodec_decode_packet(codec_ctx_.get(), &gotframe, &packet_);    //返回为负表示有错误
                        //if (decoded_ret == 0)
                        {
                            //break;
                        }    //没解压到则退出
                    }
                    if (gotframe)
                    {
                        ret = 1;
                    }
                }
                else
                {
                    //不需要解码，下次直接读packet
                    need_read_packet_ = true;
                    ret = 2;    //有数据，跳过
                }
            }
            time_pts_ = packet_.pts * time_base_packet_ - start_time_;
            time_dts_ = packet_.dts * time_base_packet_ - start_time_;
            ended_ = false;
        }
        else
        {
            ret = -1;    //流出现错误，这时有可能是已经结束或者文件损坏
            ended_ = true;
            break;
        }
        //if (ret > 0)
        //{
        //    time_pts_ = packet_.pts * time_base_packet_ - start_time_;
        //    time_dts_ = packet_.dts * time_base_packet_ - start_time_;
        //}
        if (need_read_packet_)
        {
            av_packet_unref(&packet_);
        }
        if (!til_got && ret == 0)
        {
            ret = -2;
        }
        if (decoded_ret < 0)
        {
            ret = -3;
        }
    }
    return ret;
}

void PotStream::setDecoded(bool b)
{
    decoded_ = b;
}

bool PotStream::haveDecoded()
{
    return data_map_.size() > 0;
}

void PotStream::dropAllDecoded()
{
    clearMap();
    setDecoded(false);
    need_read_packet_ = true;
}

void PotStream::resetAfterSeek(int time, int reset)
{
    av_packet_unref(&packet_);
    dropAllDecoded();
    time_dts_ = time_pts_ = time_shown_ = time;
    pause_time_ = time;
    ticks_shown_ = engine_->getTicks();
    ended_ = false;
    stopping_ = false;
    if (codec_ctx_)
    {
        avcodec_flush_buffers(codec_ctx_.get());
    }
}

FrameContent PotStream::getCurrentContent()
{
    if (!data_map_.empty())
    {
        return data_map_.begin()->second;
    }
    else
    {
        return FrameContent{ time_dts_, -1, std::monostate{} };
    }
}

//返回为非负才正常
int PotStream::openFile(const std::string& filename)
{
    //stream_index_ = -1;
    stream_ = format_ctx_->streams[stream_index_];
    if (stream_->r_frame_rate.den)
    {
        time_per_frame_ = 1e3 / av_q2d(stream_->r_frame_rate);
    }
    time_base_packet_ = 1e3 * av_q2d(stream_->time_base);
    total_time_ = format_ctx_->duration * 1e3 / AV_TIME_BASE;
    start_time_ = format_ctx_->start_time * 1e3 / AV_TIME_BASE;
    codec_ = avcodec_find_decoder(stream_->codecpar->codec_id);
    codec_ctx_.reset(avcodec_alloc_context3(codec_));
    if (!codec_)
    {
        codec_ctx_.reset();
        stream_index_ = -1;
        return -1;
    }
    if (!codec_ctx_ || avcodec_parameters_to_context(codec_ctx_.get(), stream_->codecpar) < 0)
    {
        codec_ctx_.reset();
        stream_index_ = -1;
        return -1;
    }
    codec_ctx_->pkt_timebase = stream_->time_base;
    if (avcodec_open2(codec_ctx_.get(), codec_, nullptr) < 0)
    {
        codec_ctx_.reset();
        stream_index_ = -1;
        return -1;
    }

    return stream_index_;
}

//参数为是否重置暂停时间和显示时间，一般seek后应立刻重置
int PotStream::tryDecodeFrame(bool reset)
{
    if (!exist() || !needDecode())
    {
        return -1;
    }
    int got_frame = 0;
    for (int i = 0; i < decode_frame_count_; i++)
    {
        got_frame = decodeNextPacketToFrame(true, type_ != MEDIA_TYPE_SUBTITLE);
        if (got_frame <= 0)
        {
            continue;
        }
        auto f = convertFrameToContent();
        //std::print("{}\n", _map.size());
        //如果只有一帧，则静止时间需更新
        if (data_map_.size() == 0)
        {
            if (reset)
            {
                resetTimeAxis(time_dts_);
            }
        }
        if (data_map_.count(f.time) == 0)
        {
            data_map_[f.time] = f;
        }
        setDecoded(true);
    }
    if (got_frame > 0)
    {
        return 0;
    }
    return 1;
}

void PotStream::dropDecoded()
{
    dropContent();
}

int PotStream::getTotalTime()
{
    return total_time_;
}

int PotStream::seek(int time, int direct /*= 1*/, int reset /*= 0*/)
{
    if (exist())
    {
        int c = 5;
        int64_t i = time / 1e3 * AV_TIME_BASE;

        int flag = 0;
        if (direct < 0)
        {
            flag = flag | AVSEEK_FLAG_BACKWARD;
        }
        //快速拖动时保留解码器状态，避免每次 seek 都 flush 导致卡顿；真正跳转或暂停时再重置视频解码器。
        if (type_ == MEDIA_TYPE_VIDEO && (pause_ || reset || engine_->getTicks() - seek_record_ > 100))
        {
            avcodec_flush_buffers(codec_ctx_.get());
        }
        dropAllDecoded();
        if (seek_stream_)
        {
            seek_stream_(time, direct, reset);
        }
        else
        {
            av_seek_frame(format_ctx_, -1, i, flag);
        }
        //avformat_seek_file(format_ctx_, -1, 0, i, INT64_MAX, flag);
    }
    seek_record_ = engine_->getTicks();
    Engine::getInstance()->clearAudioStream();
    return 0;
}

void PotStream::setFrameTime()
{
}

int PotStream::getTime()
{
    if (pause_)
    {
        return pause_time_;
    }
    if (exist())
    {
        return pause_time_ = std::min(int(time_shown_ + engine_->getTicks() - ticks_shown_), total_time_);
    }
    return 0;
}

int PotStream::setAnotherTime(int time)
{
    return time_other_ = time;
}

int PotStream::skipFrame(int time)
{
    int n = 0;
    while (time_dts_ < time)
    {
        n++;
        //视频需解码，因为关键帧不解后续一系列都有问题，音频可以只读不解
        if (decodeNextPacketToFrame(type_ == MEDIA_TYPE_VIDEO, type_ != MEDIA_TYPE_SUBTITLE) < 0)
        {
            break;
        }
    }
    //跳帧后需丢弃原来的解码，重置时间轴
    dropAllDecoded();
    resetTimeAxis(time_dts_);
    return n;
}

void PotStream::getSize(int& w, int& h)
{
    if (exist())
    {
        w = codec_ctx_->width;
        h = codec_ctx_->height;
    }
}

void PotStream::resetTimeAxis(int time)
{
    pause_time_ = time_shown_ = time;
    ticks_shown_ = engine_->getTicks();
}

void PotStream::setPause(bool pause)
{
    if (pause_ == pause)
    {
        return;
    }
    const int current_time = getTime();
    pause_ = pause;
    resetTimeAxis(current_time);
}

double PotStream::getRotation()
{
    if (!exist())
    {
        return 0;
    }
    double r = 0;
    auto dic = stream_->metadata;
    auto entry = av_dict_get(dic, "rotate", nullptr, 0);
    if (entry)
    {
        r = atof(entry->value);
    }
    return r;
}

void PotStream::getRatio(int& x, int& y)
{
    if (!exist())
    {
        return;
    }
    x = stream_->sample_aspect_ratio.num;
    y = stream_->sample_aspect_ratio.den;
}

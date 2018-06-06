#include "PotStream.h"
#include "PotResample.h"

PotStream::PotStream()
{
    av_register_all();
    avformat_network_init();
    format_ctx_ = avformat_alloc_context();
    frame_ = av_frame_alloc();
    //subtitle_ = av_
    //mutex_cpp.;
    time_shown_ = 0;
    ticks_shown_ = engine_->getTicks();
    av_init_packet(&packet_);
}


PotStream::~PotStream()
{
    av_frame_free(&frame_);
    if (codec_ctx_) { avcodec_close(codec_ctx_); }
    avformat_close_input(&format_ctx_);
    clearMap();
    stream_index_ = -1;
    //DestroyMutex(mutex_cpp);
}

//返回为非负才正常
int PotStream::openFile(const std::string& filename)
{
    stream_index_ = -1;
    this->filename_ = filename;
    if (avformat_open_input(&format_ctx_, filename.c_str(), nullptr, nullptr) == 0)
    {
        avformat_find_stream_info(format_ctx_, nullptr);
        for (int i = 0; i < format_ctx_->nb_streams; ++i)
        {
            if (format_ctx_->streams[i]->codec->codec_type == type_)
            {
                stream_index_vector_.push_back(i);
                if (stream_index_vector_.size() == 1)
                {
                    //printf("finded media stream: %d\n", type);
                    stream_ = format_ctx_->streams[i];
                    codec_ctx_ = stream_->codec;
                    if (stream_->r_frame_rate.den)
                    {
                        time_per_frame_ = 1e3 / av_q2d(stream_->r_frame_rate);
                    }
                    time_base_packet_ = 1e3 * av_q2d(stream_->time_base);
                    total_time_ = format_ctx_->duration * 1e3 / AV_TIME_BASE;
                    start_time_ = format_ctx_->start_time * 1e3 / AV_TIME_BASE;
                    codec_ = avcodec_find_decoder(codec_ctx_->codec_id);
                    avcodec_open2(codec_ctx_, codec_, nullptr);
                }
            }
        }
    }
    //记录下来所有的流的编号，默认同类流用一个解码器
    if (!stream_index_vector_.empty())
    {
        stream_index_ = stream_index_vector_[0];
    }
    return stream_index_;
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
    if (!exist()) { return -2; }
    int ret = 0;
    int gotframe = 0;
    int gotsize = 0;
    bool haveFrame = !need_read_packet_;
    while (ret == 0)
    {
        if (need_read_packet_)
        {
            haveFrame = av_read_frame(format_ctx_, &packet_) == 0;
        }
        if (haveFrame)
        {
            //只处理本流的数据
            if (packet_.stream_index == stream_index_)
            {
                if (decode)
                {
                    //循环处理多次才能解到一帧的情况
                    while (gotframe == 0)
                    {
                        gotsize = avcodec_decode_packet(codec_ctx_, &gotframe, &packet_);    //返回为负表示有错误
                        if (gotsize <= 0) { break; }    //没解压到则退出
                        packet_.data += gotsize;
                        packet_.size -= gotsize;
                        need_read_packet_ = packet_.size <= 0;
                        if (need_read_packet_) { break; }    //packet中已经无数据
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
            ended_ = false;
        }
        else
        {
            ret = -1;    //流出现错误，这时有可能是已经结束或者文件损坏
            ended_ = true;
            break;
        }
        if (ret > 0)
        {
            time_pts_ = packet_.pts * time_base_packet_;
            time_dts_ = packet_.dts * time_base_packet_;
        }
        if (need_read_packet_)
        {
            av_packet_unref(&packet_);
        }
        if (!til_got && ret == 0)
        {
            ret = -2;
        }
        if (gotsize < 0)
        {
            ret = -3;
        }
    }
    return ret;
}

//参数为是否重置暂停时间和显示时间，一般seek后应立刻重置
int PotStream::tryDecodeFrame(bool reset)
{
    if (!exist() || !needDecode()) { return -1; }
    int got_frame = 0;
    for (int i = 0; i < decode_frame_count_; i++)
    {
        got_frame = decodeNextPacketToFrame(true, type_ != BPMEDIA_TYPE_SUBTITLE);
        if (got_frame <= 0) { continue; }
        auto f = convertFrameToContent();
        //printf("%d\n", _map.size());
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
        //间隔比较大的情况重置播放器
        if (type_ == BPMEDIA_TYPE_VIDEO && (pause_ || reset || engine_->getTicks() - seek_record_ > 100))
        {
            avcodec_flush_buffers(codec_ctx_);
        }
        dropAllDecoded();
        av_seek_frame(format_ctx_, -1, i, flag);
    }
    seek_record_ = engine_->getTicks();
    return 0;
}

void PotStream::setFrameTime()
{

}

int PotStream::avcodec_decode_packet(AVCodecContext* ctx, int* n, AVPacket* packet)
{
    int ret;
    *n = 0;
    if (packet)
    {
        ret = avcodec_send_packet(ctx, packet);
        if (ret < 0)
        {
            return ret == AVERROR_EOF ? 0 : ret;
        }
    }

    ret = avcodec_receive_frame(ctx, frame_);
    if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
    {
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
        auto p = data_map_.begin()->second.data;
        if (p)
        {
            freeContent(p);
        }
        data_map_.erase(data_map_.begin());
    }
    return 0;
}

void PotStream::clearMap()
{
    for (auto& i : data_map_)
    {
        freeContent(i.second.data);
    }
    data_map_.clear();
}

void PotStream::setMap(int key, FrameContent f)
{
    data_map_[key] = f;
}

bool PotStream::needDecode()
{
    if (!needDecode2())
    {
        return false;
    }
    return (data_map_.size() < max_size_);
}

void PotStream::setDecoded(bool b)
{
    decoded_ = b;
}

void PotStream::dropDecoded()
{
    dropContent();
}

FrameContent PotStream::getCurrentContent()
{
    if (!data_map_.empty())
    {
        return data_map_.begin()->second;
    }
    else
    {
        return { time_dts_, -1, nullptr };
    }
}

bool PotStream::haveDecoded()
{
    return data_map_.size() > 0;
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
        if (decodeNextPacketToFrame(type_ == BPMEDIA_TYPE_VIDEO, type_ != BPMEDIA_TYPE_SUBTITLE) < 0)
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

void PotStream::dropAllDecoded()
{
    clearMap();
    setDecoded(false);
    need_read_packet_ = true;
}

void PotStream::setPause(bool pause)
{
    pause_ = pause;
    pause_time_ = getTime();
    ticks_shown_ = engine_->getTicks();
}

void PotStream::resetTimeAxis(int time)
{
    pause_time_ = time_shown_ = time;
    ticks_shown_ = engine_->getTicks();
}

double PotStream::getRotation()
{
    if (!exist()) { return 0; }
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
    if (!exist()) { return; }
    x = stream_->sample_aspect_ratio.num;
    y = stream_->sample_aspect_ratio.den;
}

void PotStream::switchStream()
{
    if (stream_index_vector_.size() <= 1) { return; }
    int i0 = -1;
    for (int i = 0; i < stream_index_vector_.size(); i++)
    {
        if (stream_index_vector_[i] == stream_index_)
        {
            i0 = i;
        }
    }
    i0 = (i0 + 1) % stream_index_vector_.size();
    stream_index_ = stream_index_vector_[i0];
}

//{
//    //避免卡死
//    if (type_ == BPMEDIA_TYPE_VIDEO && gotsize < 0)
//    {
//        BP_Event e;
//        //这里只接受QUIT和拖入事件，将其压回主序列，跳出
//        if (engine_->pollEvent(e) > 0)
//        {
//
//            if (e.type == BP_QUIT || e.type == BP_DROPFILE)
//            {
//                engine_->pushEvent(e);
//                printf("decode error.\n");
//                stopping = true;
//                break;
//            }
//        }
//    }
//}
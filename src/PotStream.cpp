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
                //printf("finded media stream: %d\n", type);
                stream_ = format_ctx_->streams[i];
                codec_ctx_ = stream_->codec;
                //timebase = av_q2d(formatCtx->streams[i]->time_base);
                if (stream_->r_frame_rate.den)
                {
                    time_per_frame_ = 1e3 / av_q2d(stream_->r_frame_rate);
                }
                time_base_packet_ = 1e3 * av_q2d(stream_->time_base);
                total_time_ = format_ctx_->duration * 1e3 / AV_TIME_BASE;
                start_time_ = format_ctx_->start_time * 1e3 / AV_TIME_BASE;
                //totalTime = (int)stream->nb_frames * timePerFrame;
                stream_index_ = i;
                codec_ = avcodec_find_decoder(codec_ctx_->codec_id);
                avcodec_open2(codec_ctx_, codec_, nullptr);
                break;
            }
        }
    }
    return stream_index_;
}

//解压帧，同时会更新当前的时间戳
//在长时间无包时可能造成界面卡死，设法改一下
int PotStream::decodeNextPacketToFrame(bool decode /*= true*/)
{
    //if (!avcodec_decode_packet) { return -1; }
    //3个状态，为正表示解到帧，为0表示还有可能解到帧，为负表示已经无帧
    if (!exist()) { return -2; }
    int ret = 0;
    int gotframe = 0;
    int gotsize = 0;
    int totalGotsize = 0;
    bool haveFrame = !need_read_packet_;
    stopping = false;
    //一帧多包，一包多帧都要考虑，甚是麻烦
    while (ret == 0)
    {
        //auto packet = new AVPacket;
        if (need_read_packet_)
        {
            haveFrame = av_read_frame(format_ctx_, &packet_) >= 0;
            decode_size_in_packet_ = 0;
        }
        if (haveFrame)
        {
            if (packet_.stream_index == stream_index_)
            {
                if (decode)
                {
                    //循环处理多次才能解到一帧的情况
                    while (gotframe == 0)
                    {
                        gotsize = avcodec_decode_packet(codec_ctx_, frame_, &gotframe, &packet_);
                        if (gotsize <= 0) { break; }
                        packet_.data += gotsize;
                        totalGotsize += gotsize;
                        packet_.size -= gotsize;
                        need_read_packet_ = packet_.size <= 0;
                        if (need_read_packet_)
                        {
                            break;
                        }
                    }
                    ret = gotframe;
                }
                else
                {
                    need_read_packet_ = true;
                    ret = 2;
                }
            }
            ended_ = false;
        }
        else
        {
            ret = -1;
            ended_ = true;
            break;
        }
        if (ret > 0)
        {
            //int totalPacketSize = decodeSizeInPacket_ + totalGotsize + packet_.size;
            //double t = decodeSizeInPacket_*time_per_packet_ / totalPacketSize;
            time_pts_ = packet_.pts * time_base_packet_;
            time_dts_ = packet_.dts * time_base_packet_;
            decode_size_in_packet_ += totalGotsize;
            //key_frame_ = frame_->key_frame;
            //frame_number_ = codecCtx_->frame_number;
            //if (type_ == 0 && key_frame_)printf("\n%dis key\n", time_dts_);
        }
        if (need_read_packet_)
        {
            av_packet_unref(&packet_);
        }
        //避免卡死
        if (gotsize < 0)
        {
            BP_Event e;
            //这里只接受QUIT和拖入事件，将其压回主序列，跳出
            if (engine_->pollEvent(e) > 0)
            {

                if (e.type == BP_QUIT || e.type == BP_DROPFILE)
                {
                    engine_->pushEvent(e);
                    printf("decode error.\n");
                    stopping = true;
                    break;
                }
            }
        }
    }
    //cout << engine_->getTicks() << '\n';
    return ret;
}


//参数为是否重置暂停时间和显示时间，一般seek后应立刻重置
int PotStream::tryDecodeFrame(bool reset)
{
    if (exist() && needDecode() && decodeNextPacketToFrame() > 0)
    {
        auto f = convertFrameToContent();
        if (useMap())
        {
            //printf("%d\n", _map.size());
            //如果只有一帧，则静止时间需更新
            if (data_map_.size() == 0)
            {
                if (reset)
                {
                    resetTimeAxis(time_dts_);
                }
            }
            if (data_map_.count(f.time) == 0 && f.data)
            {
                data_map_[f.time] = f;
            }
        }
        else
        {
            if (reset)
            {
                resetTimeAxis(time_dts_);
            }
        }
        setDecoded(true);
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
        if (type_ == BPMEDIA_TYPE_VIDEO
            && (pause_ || reset || engine_->getTicks() - seek_record_ > 100))
        {
            avcodec_flush_buffers(codec_ctx_);
        }
        dropAllDecoded();
        av_seek_frame(format_ctx_, -1, i, flag);

        //decodeFrame(true);
    }
    seek_record_ = engine_->getTicks();
    return 0;
}

void PotStream::setFrameTime()
{

}

int PotStream::dropContent(int key)
{
    mutex_.lock();
    if (data_map_.size() > 0)
    {
        auto p = data_map_.begin()->second.data;
        if (p)
        {
            freeContent(p);
        }
        data_map_.erase(data_map_.begin());
    }
    mutex_.unlock();
    return 0;
}

void PotStream::clearMap()
{
    //SDL_LockMutex(mutex_cpp);
    //printf("clear buffer begin with %d\n", _map.size());
    //for (auto i = _map.begin(); i != _map.end(); i++)
    mutex_.lock();
    for (auto& i : data_map_)
    {
        freeContent(i.second.data);
    }
    data_map_.clear();
    mutex_.unlock();
    //printf("clear buffer end with %d\n", _map.size());
    //SDL_UnlockMutex(mutex_cpp);
}

void PotStream::setMap(int key, Content f)
{
    data_map_[key] = f;
}

bool PotStream::needDecode()
{
    if (!needDecode2())
    {
        return false;
    }
    if (useMap())
    {
        return (data_map_.size() < max_size_);
    }
    else
    {
        return !decoded_;
    }
}

void PotStream::setDecoded(bool b)
{
    decoded_ = b;
}

void PotStream::dropDecoded()
{
    if (useMap())
    {
        dropContent();
    }
    else
    {
        decoded_ = false;
    }
}

bool PotStream::useMap()
{
    return max_size_ > 0;
}

PotStream::Content PotStream::getCurrentContent()
{
    if (useMap())
    {
        if (data_map_.size() > 0)
        {
            return data_map_.begin()->second;
        }
        else
            return{ -1, -1, nullptr };
    }
    else
    {
        return{ time_dts_, data_length_, data_ };
    }
}

bool PotStream::haveDecoded()
{
    if (useMap())
    {
        return data_map_.size() > 0;
    }
    else
    {
        return decoded_;
    }
}


int PotStream::getTime()
{
    if (pause_)
    {
        return pause_time_;
    }
    //if (type_== BPMEDIA_TYPE_AUDIO)
    //printf("%d//%d//%d//\n", time_shown_, ticks_shown_, engine_->getTicks());
    //if (exist() && !_ended)
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
        if (decodeNextPacketToFrame(type_ == BPMEDIA_TYPE_VIDEO) < 0)
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


#include "BigPotStream.h"
#include "BigPotResample.h"

BigPotStream::BigPotStream()
{
    av_register_all();
    formatCtx_ = avformat_alloc_context();
    frame_ = av_frame_alloc();
    //subtitle_ = av_
    //mutex_cpp.;
    time_shown_ = 0;
    ticks_shown_ = engine_->getTicks();
}


BigPotStream::~BigPotStream()
{
    av_frame_free(&frame_);
    avformat_close_input(&formatCtx_);
    clearMap();
    stream_index_ = -1;
    //DestroyMutex(mutex_cpp);
}

//返回为非负才正常
int BigPotStream::openFile(const std::string& filename)
{
    stream_index_ = -1;
    this->filename_ = filename;
    if (avformat_open_input(&formatCtx_, filename.c_str(), nullptr, nullptr) == 0)
    {
        avformat_find_stream_info(formatCtx_, nullptr);
        for (int i = 0; i < formatCtx_->nb_streams; ++i)
        {
            if (formatCtx_->streams[i]->codec->codec_type == type_)
            {
                //printf("finded media stream: %d\n", type);
                stream_ = formatCtx_->streams[i];
                codecCtx_ = stream_->codec;
                //timebase = av_q2d(formatCtx->streams[i]->time_base);
                if (stream_->r_frame_rate.den)
                { time_per_frame_ = 1e3 / av_q2d(stream_->r_frame_rate); }
                time_base_packet_ = 1e3 * av_q2d(stream_->time_base);
                total_time_ = formatCtx_->duration * 1e3 / AV_TIME_BASE;
                start_time_ = formatCtx_->start_time * 1e3 / AV_TIME_BASE;
                //totalTime = (int)stream->nb_frames * timePerFrame;
                stream_index_ = i;
                codec_ = avcodec_find_decoder(codecCtx_->codec_id);
                avcodec_open2(codecCtx_, codec_, nullptr);
                break;
            }
        }
    }
    //可以写在子类中的
    switch (type_)
    {
    case BPMEDIA_TYPE_VIDEO:
        avcodec_decode_packet = &avcodec_decode_video2;
        break;
    case BPMEDIA_TYPE_AUDIO:
        avcodec_decode_packet = &avcodec_decode_audio4;
        break;
    case BPMEDIA_TYPE_SUBTITLE:
        avcodec_decode_packet_subtitle = &avcodec_decode_subtitle2;
        break;
    }
    return stream_index_;
}

//解压帧，同时会更新当前的时间戳
//在长时间无包时可能造成界面卡死，设法改一下
int BigPotStream::decodeNextPacketToFrame(bool decode /*= true*/)
{
    if (!avcodec_decode_packet) { return -1; }
    //3个状态，为正表示解到帧，为0表示还有可能解到帧，为负表示已经无帧
    if (!exist()) { return -2; }
    int ret = 0;
    int gotframe = 0;
    int gotsize = 0;
    int totalGotsize = 0;
    bool haveFrame = !needReadPacket_;
    //一帧多包，一包多帧都要考虑，甚是麻烦
    while (ret == 0)
    {
        BP_Event e;
        //这里只接受QUIT和拖入事件，将其压回主序列，跳出
        engine_->pollEvent(e);
        {
            if (e.type == BP_QUIT || e.type == BP_DROPFILE)
            {
                engine_->pushEvent(e);
                break;
            }
        }
        //auto packet = new AVPacket;
        if (needReadPacket_)
        {
            haveFrame = av_read_frame(formatCtx_, &packet_) >= 0;
            decodeSizeInPacket_ = 0;
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
                        gotsize = avcodec_decode_packet(codecCtx_, frame_, &gotframe, &packet_);
                        if (gotsize <= 0) { break; }
                        packet_.data += gotsize;
                        totalGotsize += gotsize;
                        packet_.size -= gotsize;
                        needReadPacket_ = packet_.size <= 0;
                        if (needReadPacket_)
                        { break; }
                    }
                    ret = gotframe;
                }
                else
                {
                    needReadPacket_ = true;
                    ret = 2;
                }
            }
            _ended = false;
        }
        else
        {
            ret = -1;
            _ended = true;
            break;
        }
        if (ret > 0)
        {
            //int totalPacketSize = decodeSizeInPacket_ + totalGotsize + packet_.size;
            //double t = decodeSizeInPacket_*time_per_packet_ / totalPacketSize;
            time_pts_ = packet_.pts * time_base_packet_;
            time_dts_ = packet_.dts * time_base_packet_;
            decodeSizeInPacket_ += totalGotsize;
            //key_frame_ = frame_->key_frame;
            //frame_number_ = codecCtx_->frame_number;
            //if (type_ == 0 && key_frame_)printf("\n%dis key\n", time_dts_);
        }
        if (needReadPacket_)
        { av_free_packet(&packet_); }
    }
    //cout << engine_->getTicks() << '\n';
    return ret;
}


//参数为是否重置暂停时间和显示时间，一般seek后应立刻重置
int BigPotStream::tryDecodeFrame(bool reset)
{
    if (exist() && needDecode() && decodeNextPacketToFrame() > 0)
    {
        auto f = convertFrameToContent();
        if (useMap())
        {
            //printf("%d\n", _map.size());
            //如果只有一帧，则静止时间需更新
            if (_map.size() == 0)
            {
                if (reset)
                { resetTimeAxis(time_dts_); }
            }
            if (_map.count(f.time) == 0 && f.data)
            { _map[f.time] = f; }
        }
        else
        {
            if (reset)
            { resetTimeAxis(time_dts_); }
        }
        setDecoded(true);
        return 0;
    }
    return 1;
}


int BigPotStream::getTotalTime()
{
    return total_time_;
}

int BigPotStream::seek(int time, int direct /*= 1*/, int reset /*= 0*/)
{
    if (exist())
    {
        int c = 5;
        int64_t i = time / 1e3 * AV_TIME_BASE;

        int flag = 0;
        if (direct < 0)
        { flag = flag | AVSEEK_FLAG_BACKWARD; }
        //间隔比较大的情况重置播放器
        if (type_ == BPMEDIA_TYPE_VIDEO
            && (pause_ || reset || engine_->getTicks() - _seek_record > 100))
        {
            avcodec_flush_buffers(codecCtx_);
        }
        dropAllDecoded();
        av_seek_frame(formatCtx_, -1, i, flag);

        //decodeFrame(true);
    }
    _seek_record = engine_->getTicks();
    return 0;
}

void BigPotStream::setFrameTime()
{

}

int BigPotStream::dropContent(int key)
{
    mutex_.lock();
    if (_map.size() > 0)
    {
        auto p = _map.begin()->second.data;
        if (p)
        {
            freeContent(p);
        }
        _map.erase(_map.begin());
    }
    mutex_.unlock();
    return 0;
}

void BigPotStream::clearMap()
{
    //SDL_LockMutex(mutex_cpp);
    //printf("clear buffer begin with %d\n", _map.size());
    //for (auto i = _map.begin(); i != _map.end(); i++)
    mutex_.lock();
    for (auto& i : _map)
    {
        freeContent(i.second.data);
    }
    _map.clear();
    mutex_.unlock();
    //printf("clear buffer end with %d\n", _map.size());
    //SDL_UnlockMutex(mutex_cpp);
}

void BigPotStream::setMap(int key, Content f)
{
    _map[key] = f;
}

bool BigPotStream::needDecode()
{
    if (!needDecode2())
    { return false; }
    if (useMap())
    { return (_map.size() < maxSize_); }
    else
    { return !_decoded; }
}

void BigPotStream::setDecoded(bool b)
{
    _decoded = b;
}

void BigPotStream::dropDecoded()
{
    if (useMap())
    { dropContent(); }
    else
    { _decoded = false; }
}

bool BigPotStream::useMap()
{
    return maxSize_ > 0;
}

BigPotStream::Content BigPotStream::getCurrentContent()
{
    if (useMap())
    {
        if (_map.size() > 0)
        { return _map.begin()->second; }
        else
            return{ -1, -1, nullptr };
    }
    else
    {
        return{ time_dts_, data_length_, data_ };
    }
}

bool BigPotStream::haveDecoded()
{
    if (useMap())
    {
        return _map.size() > 0;
    }
    else
    {
        return _decoded;
    }
}


int BigPotStream::getTime()
{
    if (pause_)
    { return pause_time_; }
    //if (type_== BPMEDIA_TYPE_AUDIO)
    //printf("%d//%d//%d//\n", time_shown_, ticks_shown_, engine_->getTicks());
    //if (exist() && !_ended)
    if (exist())
    { return pause_time_ = std::min(int(time_shown_ + engine_->getTicks() - ticks_shown_), total_time_); }
    return 0;
}

int BigPotStream::setAnotherTime(int time)
{
    return time_other_ = time;
}

int BigPotStream::skipFrame(int time)
{
    int n = 0;
    while (time_dts_ < time)
    {
        n++;
        //视频需解码，因为关键帧不解后续一系列都有问题，音频可以只读不解
        if (decodeNextPacketToFrame(type_ == BPMEDIA_TYPE_VIDEO) < 0)
        { break; }
    }
    //跳帧后需丢弃原来的解码，重置时间轴
    dropAllDecoded();
    resetTimeAxis(time_dts_);
    return n;
}

void BigPotStream::getSize(int& w, int& h)
{
    if (exist())
    {
        w = codecCtx_->width;
        h = codecCtx_->height;
    }
}

void BigPotStream::dropAllDecoded()
{
    clearMap();
    setDecoded(false);
    needReadPacket_ = true;
}

void BigPotStream::setPause(bool pause)
{
    pause_ = pause;
    pause_time_ = getTime();
    ticks_shown_ = engine_->getTicks();
}

void BigPotStream::resetTimeAxis(int time)
{
    pause_time_ = time_shown_ = time;
    ticks_shown_ = engine_->getTicks();
}

double BigPotStream::getRotation()
{
    if (!exist()) { return 0; }
    double r = 0;
    auto dic = stream_->metadata;
    auto entry = av_dict_get(dic, "rotate", nullptr, 0);
    if (entry)
    { r = atof(entry->value); }
    return r;
}

void BigPotStream::getRatio(int& x, int& y)
{
    if (!exist()) { return; }
    x = stream_->sample_aspect_ratio.num;
    y = stream_->sample_aspect_ratio.den;
}


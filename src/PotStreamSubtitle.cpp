#include "PotStreamSubtitle.h"
#include "PotSubtitleManager.h"
#include "Timer.h"
#include "strfunc.h"

PotStreamSubtitle::PotStreamSubtitle()
{
    type_ = MEDIA_TYPE_SUBTITLE;
    sub_ = PotSubtitleManager::createSubtitle(" .ass");
}

PotStreamSubtitle::~PotStreamSubtitle()
{
    delete sub_;
}

int PotStreamSubtitle::show(int time)
{
    if (exist() && sub_ && sub_->exist() && sub_->show(time) || time + 5000 >= time_dts_)
    {
        dropDecoded();
        return 1;
    }
    return 0;
}

void PotStreamSubtitle::setFrameSize(int w, int h)
{
    if (sub_)
    {
        sub_->setFrameSize(w, h);
    }
}

int PotStreamSubtitle::openFile(const std::string& filename)
{
    int ret = PotStream::openFile(filename);
    if (ret >= 0 && !sub_inited_)
    {
        sub_inited_ = true;
        if (codec_ctx_->codec_id == AV_CODEC_ID_ASS)
        {
            //sub_ = PotSubtitleManager::createSubtitle(".ass");
        }
        else
        {
            //sub_ = PotSubtitleManager::createSubtitle(".ass");
        }
        if (sub_ && codec_ctx_->subtitle_header_size > 0)
        {
            sub_->openSubtitleFromMem((char*)codec_ctx_->subtitle_header);
        }
    }
    return ret;
}

void PotStreamSubtitle::clear()
{
    if (sub_)
    {
        sub_->clear();
    }
}

int PotStreamSubtitle::avcodec_decode_packet(AVCodecContext* cont, int* n, AVPacket* packet)
{
    int ret = avcodec_decode_subtitle2(cont, &avsubtitle_, n, packet);
    return ret;
}

FrameContent PotStreamSubtitle::convertFrameToContent()
{
    if (sub_ && avsubtitle_.num_rects > 0)
    {
        auto& rect = avsubtitle_.rects[0];
        if (rect->ass)
        {
            sub_->readOne(rect->ass,
                1.0 * avsubtitle_.pts / AV_TIME_BASE * 1000 + avsubtitle_.start_display_time,
                1.0 * avsubtitle_.pts / AV_TIME_BASE * 1000 + avsubtitle_.end_display_time);
        }
    }
    return { time_dts_, 0, nullptr };
}

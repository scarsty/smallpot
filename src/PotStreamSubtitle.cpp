#include "PotStreamSubtitle.h"
#include "PotSubtitleManager.h"

PotStreamSubtitle::PotStreamSubtitle()
{
    type_ = BPMEDIA_TYPE_SUBTITLE;
}

PotStreamSubtitle::~PotStreamSubtitle()
{
    delete sub_;
}

void PotStreamSubtitle::show(int time)
{
    if (exist() && sub_ && sub_->show(time) || time + 5000 >= time_dts_)
    {
        dropDecoded();
    }
}

void PotStreamSubtitle::setFrameSize(int w, int h)
{
    if (sub_)
    {
        sub_->setFrameSize(w, h);
    }
}

int PotStreamSubtitle::avcodec_decode_packet(AVCodecContext* cont, void* subtitle, int* n, AVPacket* packet)
{
    int ret = avcodec_decode_subtitle2(cont, &avsubtitle_, n, packet);
    return ret;
}

PotStream::Content PotStreamSubtitle::convertFrameToContent(void* p /*= nullptr*/)
{
    if (sub_ && avsubtitle_.num_rects > 0)
    {
        auto& rect = avsubtitle_.rects[0];
        if (rect->ass)
        {
            sub_->readOne(rect->ass);
        }
    }
    return { time_dts_, 0, nullptr };
}

int PotStreamSubtitle::openFile(const std::string& filename)
{
    int ret = PotStream::openFile(filename);
    if (ret >= 0 && !sub_inited_)
    {
        sub_inited_ = true;
        if (codec_ctx_->codec_id == AV_CODEC_ID_ASS)
        {
            sub_ = PotSubtitleManager::createSubtitle(".ass");

        }
        if (sub_)
        {
            sub_->openSubtitleFromMem((char*)codec_ctx_->extradata);
        }
    }
    return ret;
}

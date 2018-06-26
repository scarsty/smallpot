#include "PotSubtitleAss.h"
#include "PotConv.h"


PotSubtitleAss::PotSubtitleAss()
{
    init();
}


PotSubtitleAss::~PotSubtitleAss()
{
    destroyAllTex();
    closeSubtitle();
    destroy();
}

void PotSubtitleAss::init()
{
    library_ = ass_library_init();
    renderer_ = ass_renderer_init(library_);
    ass_set_fonts(renderer_, fontname_.c_str(), "Sans", 1, "", 0);
}

bool PotSubtitleAss::openSubtitle(const std::string& filename)
{
    //函数的参数是char*,为免意外复制一份
    auto s = filename;
    //if (checkFileExt(filename))
    track_ = ass_read_file(library_, (char*)s.c_str(), NULL);
    haveSubtitle_ = (track_ != nullptr);
    if (haveSubtitle_) { subfilename_ = filename; }
    return  haveSubtitle_;
}

int PotSubtitleAss::show(int time)
{
    int a;
    image_ = ass_render_frame(renderer_, track_, time, &a);
    //cout << engine_->getTicks() << endl;
    auto img = image_;
    if (a)
    {
        destroyAllTex();
        while (img)
        {
            auto t = engine_->transBitmapToTexture(img->bitmap, img->color, img->w, img->h, img->stride);
            engine_->renderCopy(t, img->dst_x, img->dst_y, img->w, img->h, 1);
            tex_vector_.push_back(t);
            img = img->next;
        }
    }
    else
    {
        int i = 0;
        while (img && i < tex_vector_.size())
        {
            engine_->renderCopy(tex_vector_[i++], img->dst_x, img->dst_y, img->w, img->h, 1);
            img = img->next;
        }
    }
    return a;
    //cout << engine_->getTicks() << endl;
}

void PotSubtitleAss::destroy()
{
    ass_renderer_done(renderer_);
    ass_library_done(library_);
}

void PotSubtitleAss::setFrameSize(int w, int h)
{
    //if (_track)
    ass_set_frame_size(renderer_, w, h);
}

void PotSubtitleAss::openSubtitleFromMem(const std::string& str)
{
    track_ = ass_read_memory(library_, (char*)str.c_str(), str.size(), NULL);
}

void PotSubtitleAss::readOne(const std::string& str, int start_time, int end_time)
{
    if (contents_.count(str) == 0)
    {
        contents_.insert(str);
        ass_process_data(track_, (char*)str.c_str(), str.size());
#ifdef _DEBUG
        printf("%s\n", PotConv::conv(str, "utf-8", "cp936").c_str());
#endif
    }
}

void PotSubtitleAss::clear()
{
    ass_flush_events(track_);
}

void PotSubtitleAss::closeSubtitle()
{
    if (track_)
    {
        ass_free_track(track_);
    }
    track_ = nullptr;
}

void PotSubtitleAss::destroyAllTex()
{
    for (auto t : tex_vector_)
    {
        engine_->destroyTexture(t);
    }
    tex_vector_.clear();
}
#pragma once
#include "PotSubtitle.h"

class PotSubtitleAss :
    public PotSubtitle
{
public:
    PotSubtitleAss();
    virtual ~PotSubtitleAss();
private:
    ASS_Library* library_ = nullptr;
    ASS_Renderer* renderer_ = nullptr;
    ASS_Track* track_ = nullptr;
    ASS_Image* image_ = nullptr;

    std::vector<BP_Texture*> tex_vector_;
    void destroyAllTex();

public:
    virtual void init();
    virtual void destroy();
    virtual bool openSubtitle(const std::string& filename);
    virtual void closeSubtitle();
    virtual void show(int time);
    virtual void setFrameSize(int w, int h);
};


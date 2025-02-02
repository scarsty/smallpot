#pragma once
#include "PotSubtitle.h"

class PotSubtitleAss : public PotSubtitle
{
public:
    PotSubtitleAss();
    virtual ~PotSubtitleAss();

private:
    ASS_Library* library_ = nullptr;
    ASS_Renderer* renderer_ = nullptr;
    ASS_Track* track_ = nullptr;
    ASS_Image* image_ = nullptr;

    std::vector<Texture*> tex_vector_;
    void destroyAllTex();

public:
    virtual void init() override;
    virtual void destroy() override;
    virtual bool openSubtitle(const std::string& filename) override;
    virtual void closeSubtitle() override;
    virtual int show(int time) override;
    virtual void setFrameSize(int w, int h) override;
    virtual void openSubtitleFromMem(const std::string& str) override;
    virtual void readOne(const std::string& str, int start_time, int end_time) override;
    virtual void clear() override;
};

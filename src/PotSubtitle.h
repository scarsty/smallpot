#pragma once
#include "PotBase.h"

extern "C"
{
#include "ass/ass.h"
#include "ass/ass_types.h"
}

#include <vector>

enum PotPSubtitleType
{
    BPSUB_TYPE_ASS,
    BPSUB_TYPE_SRT
};

class PotSubtitle :
    public PotBase
{
public:
    PotSubtitle();
    virtual ~PotSubtitle();
protected:
    PotPSubtitleType type_;
    std::string subfilename_, fontname_;
    bool haveSubtitle_ = false;
public:
    bool exist() { return haveSubtitle_; };
    bool reOpenSubtitle() { return openSubtitle(subfilename_); };

    virtual void init() {};
    virtual bool openSubtitle(const std::string& filename) { return false; };
    virtual void closeSubtitle() {};
    virtual void show(int time) {};
    virtual void destroy() {};
    virtual void setFrameSize(int w, int h) {};

};

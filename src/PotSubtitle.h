#pragma once
#include "PotBase.h"

extern "C"
{
#include "ass/ass.h"
#include "ass/ass_types.h"
}

#include <vector>
#include <set>

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
    std::set<std::string> contents_;
public:
    bool exist() { return haveSubtitle_; }
    bool reOpenSubtitle() { return openSubtitle(subfilename_); }

    virtual void init() {};
    virtual bool openSubtitle(const std::string& filename) { return false; }
    virtual void closeSubtitle() {}
    virtual int show(int time) { return 1; }
    virtual void destroy() {}
    virtual void setFrameSize(int w, int h) {}
    virtual void openSubtitleFromMem(const std::string& str) {}
    virtual void readOne(const std::string& str) {}
    virtual void clear() {}
};

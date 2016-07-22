#pragma once
#include "BigPotBase.h"

extern "C"
{
#include "ass/ass.h"
#include "ass/ass_types.h"
}

#include <vector>

#include "BigPotString.h"


enum BigPotPSubtitleType
{
    BPSUB_TYPE_ASS,
    BPSUB_TYPE_SRT
};

class BigPotSubtitle :
    public BigPotBase
{
public:
    BigPotSubtitle();
    virtual ~BigPotSubtitle();
protected:
    BigPotPSubtitleType type_;
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

//下面是字幕工厂
private:
    static std::vector<std::string> _ext;

public:
    static BigPotSubtitle* createSubtitle(const std::string& filename);
    static void destroySubtitle(BigPotSubtitle*& sub) { delete sub; sub = nullptr; }
    static std::string lookForSubtitle(const std::string& filename);

    static bool tryOpenSubtitle(const std::string& filename);
    static bool isSubtitle(const std::string& filename);
};

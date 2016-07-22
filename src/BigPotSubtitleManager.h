#pragma once
#include <string>
#include "BigPotSubtitle.h"
#include "BigPotBase.h"

class BigPotSubtitleManager : BigPotBase
{
public:
    BigPotSubtitleManager();
    virtual ~BigPotSubtitleManager();

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


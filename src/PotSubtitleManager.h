#pragma once
#include <string>
#include "PotSubtitle.h"
#include "PotBase.h"

class PotSubtitleManager : PotBase
{
public:
    PotSubtitleManager();
    virtual ~PotSubtitleManager();

    //下面是字幕工厂
private:
    static std::vector<std::string> ext_names_;

public:
    static PotSubtitle* createSubtitle(const std::string& filename);
    static void destroySubtitle(PotSubtitle*& sub) { delete sub; sub = nullptr; }
    static std::string lookForSubtitle(const std::string& filename);

    static bool tryOpenSubtitle(const std::string& filename);
    static bool isSubtitle(const std::string& filename);
};


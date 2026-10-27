#include "PotSubtitle.h"
#include "Config.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleSrt.h"
#include "filefunc.h"

PotSubtitle::PotSubtitle()
{
    subfilename_ = Config::getInstance()["sub_font"].toString();
    if (!filefunc::fileExist(subfilename_))
    {
#ifdef _WIN32
        subfilename_ = "C:/Windows/Fonts/msyh.ttc";
#else
        subfilename_ = "/System/Library/Fonts/Palatino.ttc";
#endif
    }
}

PotSubtitle::~PotSubtitle()
{
}

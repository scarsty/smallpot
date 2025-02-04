#include "PotSubtitle.h"
#include "Config.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleSrt.h"
#include "filefunc.h"

PotSubtitle::PotSubtitle()
{
#ifdef _WIN32
    fontpath_ = R"(C:/Program Files/Microsoft Office/root/vfs/Fonts/private,C:/Windows/Fonts)";
#endif
#ifdef __APPLE__
    fontpath_ = "/System/Library/Fonts";
#endif
    auto fontpath = Config::getInstance()["sub_font"].toString();
    if (filefunc::fileExist(fontpath))
    {
        fontpath_ += fontpath + "," + fontpath_;
    }
}

PotSubtitle::~PotSubtitle()
{
    if (Config::getInstance()["sub_font"].toString() == "")
    {
        Config::getInstance()["sub_font"] = fontpath_;
    }
}

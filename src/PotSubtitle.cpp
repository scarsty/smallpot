#include "PotSubtitle.h"
#include "Config.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleSrt.h"
#include "filefunc.h"

PotSubtitle::PotSubtitle()
{
    fontname_ = Config::getInstance()->getString("sub_font");
    if (!filefunc::fileExist(fontname_))
    {
#ifdef _WIN32
        fontname_ = "C:\\Windows\\Fonts\\msyh.ttc";
#endif
#ifdef __APPLE__
        fontname_ = "/System/Library/Fonts/STHeiti Medium.ttc";
#endif
    }
}

PotSubtitle::~PotSubtitle()
{
    if (Config::getInstance()->getString("sub_font") == "")
    {
        Config::getInstance()->setString("sub_font", fontname_);
    }
}

#include "BigPotSubtitle.h"
#include "BigPotSubtitleAss.h"
#include "BigPotSubtitleSrt.h"
#include "Config.h"
#include "File.h"

BigPotSubtitle::BigPotSubtitle()
{
    fontname_ = config_->getString("sub_font");
    if (!File::fileExist(fontname_))
    {
#ifdef _WIN32
        fontname_ = "c:/windows/fonts/msyh.ttc";
#endif
#ifdef __APPLE__
        fontname_ = "/System/Library/Fonts/STHeiti Medium.ttc";
#endif
    }
}


BigPotSubtitle::~BigPotSubtitle()
{
    if (config_->getString("sub_font") == "")
    { config_->setString(fontname_, "sub_font"); }
}

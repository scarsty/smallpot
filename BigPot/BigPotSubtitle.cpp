#include "BigPotSubtitle.h"

BigPotSubtitle::BigPotSubtitle()
{
#ifdef _WIN32
	fontname_ = "c:/windows/fonts/msyh.ttc";
#endif
#ifdef __APPLE__
	fontname_ = "/System/Library/Fonts/STHeiti Medium.ttc";
#endif
	if (config_->getString("sub_font") != "")
	{
		auto name = config_->getString("sub_font");
		if (fileExist(name))
		{
			fontname_ = name;
		}
	}
}


BigPotSubtitle::~BigPotSubtitle()
{
}



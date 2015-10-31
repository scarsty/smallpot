#include "BigPotSubtitle.h"

BigPotSubtitle::BigPotSubtitle()
{
#ifdef _WIN32
	auto name = "c:/windows/fonts/msyh.ttc";
	if (fileExist(name))
	{
		fontname_ = name;
	}
	else
	{
		fontname_ = changeFileExt(name, "ttf");
	}
#endif
#ifdef __APPLE__
	fontname_ = "/System/Library/Fonts/STHeiti Medium.ttc";
#endif

}


BigPotSubtitle::~BigPotSubtitle()
{
}



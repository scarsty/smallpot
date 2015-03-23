#include "BigPotSubtitle.h"


BigPotSubtitle::BigPotSubtitle()
{
#ifdef _MSC_VER
	fontname_ = "c:/windows/fonts/msyh.ttf";
#endif
#ifdef __APPLE__
	fontname_ = "/System/Library/Fonts/STHeiti Medium.ttc";
#endif

}


BigPotSubtitle::~BigPotSubtitle()
{
}



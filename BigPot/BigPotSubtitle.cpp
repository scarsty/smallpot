#include "BigPotSubtitle.h"


BigPotSubtitle::BigPotSubtitle()
{
#ifdef _MSC_VER
	fontname_ = "c:/windows/fonts/msyh.ttf";
#endif
#ifdef __APPLE__
	fontname_ = "/library/fonts/Heiti.ttc";
#endif

}


BigPotSubtitle::~BigPotSubtitle()
{
}



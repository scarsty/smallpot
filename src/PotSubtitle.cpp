#include "PotSubtitle.h"
#include "Config.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleSrt.h"
#include "filefunc.h"

PotSubtitle::PotSubtitle()
{
    subfilename_ = Config::getInstance()["sub_font"].toString();
}

PotSubtitle::~PotSubtitle()
{
}

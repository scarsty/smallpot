#include "BigPotSubtitle.h"
#include "BigPotSubtitleAss.h"
#include "BigPotSubtitleSrt.h"
#include "BigPotConfig.h"

BigPotSubtitle::BigPotSubtitle()
{
    fontname_ = config_->getString("sub_font");
    if (!fileExist(fontname_))
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

std::vector<std::string> BigPotSubtitle::_ext;

BigPotSubtitle* BigPotSubtitle::createSubtitle(const std::string& filename)
{
    BigPotSubtitle* ret = nullptr;
    auto ext = toLowerCase(getFileExt(filename));
    if (ext == "ass" || ext == "ssa")
    { ret = new BigPotSubtitleAss; }
    else if (ext == "srt" || ext == "txt")
    { ret = new BigPotSubtitleSrt; }
    if (ret)
    {
        //ret->init();
        ret->openSubtitle(filename);
    }
    else
    {
        ret = new BigPotSubtitle;
    }
    return ret;
}

std::string BigPotSubtitle::lookForSubtitle(const std::string& filename)
{
    if (_ext.size() == 0)
    {
        _ext.push_back("ass");
        _ext.push_back("ssa");
        _ext.push_back("srt");
        _ext.push_back("txt");
    }

    std::string str = "";
    bool b = false;
    //检查默认类型
    for (auto& ext : _ext)
    {
        str = changeFileExt(filename, ext);
        if (fileExist(str))
        {
            return str;
        }
    }
    str = fingFileWithMainName(filename);
    if (!isSubtitle(str))
    { str = ""; }
    return str;
}

bool BigPotSubtitle::tryOpenSubtitle(const std::string& filename)
{
    bool b = false;
    return b;
}

bool BigPotSubtitle::isSubtitle(const std::string& filename)
{
    auto ext = getFileExt(filename);
    ext = toLowerCase(ext);
    //transform(ext.begin(), ext.end(), ext.begin(), tolower);
    bool b = false;
    for (auto& e : _ext)
    {
        if (e == ext)
        {
            b = true;
            break;
        }
    }
    return b;
}
#include "BigPotSubtitleManager.h"
#include "File.h"
#include "BigPotSubtitleAss.h"
#include "BigPotSubtitleSrt.h"

std::vector<std::string> BigPotSubtitleManager::_ext;

BigPotSubtitleManager::BigPotSubtitleManager()
{
}


BigPotSubtitleManager::~BigPotSubtitleManager()
{
}

BigPotSubtitle* BigPotSubtitleManager::createSubtitle(const std::string& filename)
{
    BigPotSubtitle* ret = nullptr;
    auto ext = File::toLowerCase(File::getFileExt(filename));
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

std::string BigPotSubtitleManager::lookForSubtitle(const std::string& filename)
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
        str = File::changeFileExt(filename, ext);
        if (File::fileExist(str))
        {
            return str;
        }
    }
    str = File::fingFileWithMainName(filename);
    if (!isSubtitle(str))
    { str = ""; }
    return str;
}

bool BigPotSubtitleManager::tryOpenSubtitle(const std::string& filename)
{
    bool b = false;
    return b;
}

bool BigPotSubtitleManager::isSubtitle(const std::string& filename)
{
    auto ext = File::getFileExt(filename);
    ext = File::toLowerCase(ext);
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
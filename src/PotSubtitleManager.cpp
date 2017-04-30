#include "PotSubtitleManager.h"
#include "File.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleSrt.h"

std::vector<std::string> PotSubtitleManager::_ext;

PotSubtitleManager::PotSubtitleManager()
{
}


PotSubtitleManager::~PotSubtitleManager()
{
}

PotSubtitle* PotSubtitleManager::createSubtitle(const std::string& filename)
{
    PotSubtitle* ret = nullptr;
    auto ext = File::toLowerCase(File::getFileExt(filename));
    if (ext == "ass" || ext == "ssa")
    { ret = new PotSubtitleAss; }
    else if (ext == "srt" || ext == "txt")
    { ret = new PotSubtitleSrt; }
    if (ret)
    {
        //ret->init();
        ret->openSubtitle(filename);
    }
    else
    {
        ret = new PotSubtitle;
    }
    return ret;
}

std::string PotSubtitleManager::lookForSubtitle(const std::string& filename)
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

bool PotSubtitleManager::tryOpenSubtitle(const std::string& filename)
{
    bool b = false;
    return b;
}

bool PotSubtitleManager::isSubtitle(const std::string& filename)
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
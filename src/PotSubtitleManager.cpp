#include "PotSubtitleManager.h"
#include "File.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleSrt.h"

std::vector<std::string> PotSubtitleManager::ext_names_;

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
    {
        ret = new PotSubtitleAss;
    }
    else if (ext == "srt" || ext == "txt")
    {
        ret = new PotSubtitleSrt;
    }
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
    if (ext_names_.size() == 0)
    {
        ext_names_.push_back("ass");
        ext_names_.push_back("ssa");
        ext_names_.push_back("srt");
        ext_names_.push_back("txt");
    }

    std::string str = "";
    bool b = false;
    //检查默认类型
    for (auto& ext : ext_names_)
    {
        str = File::changeFileExt(filename, ext);
        if (File::fileExist(str))
        {
            return str;
        }
    }
    str = File::fingFileWithMainName(filename);
    if (!isSubtitle(str))
    {
        str = "";
    }
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
    for (auto& e : ext_names_)
    {
        if (e == ext)
        {
            b = true;
            break;
        }
    }
    return b;
}
#include "File.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleManager.h"
#include "PotSubtitleSrt.h"
#include "libconvert.h"

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
    auto ext = convert::convertCase(File::getFileExt(filename));
    if (ext == "ass" || ext == "ssa")
    {
        ret = new PotSubtitleAss();
    }
    else if (ext == "srt" || ext == "txt")
    {
        ret = new PotSubtitleSrt();
    }
    if (ret)
    {
        //ret->init();
        if (File::getFileMainname(File::getFilenameWithoutPath(filename)) != "")
        {
            ret->openSubtitle(filename);
        }
    }
    else
    {
        ret = new PotSubtitle();
    }
    return ret;
}

PotSubtitle* PotSubtitleManager::createSubtitle(AVSubtitleType type)
{
    PotSubtitle* ret = nullptr;
    switch (type)
    {
    case SUBTITLE_NONE:
        break;
    case SUBTITLE_BITMAP:
        break;
    case SUBTITLE_TEXT:
        break;
    case SUBTITLE_ASS:
        ret = new PotSubtitleAss();
        break;
    default:
        break;
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
        str = File::getFilePath(str) + "/sub/" + File::getFilenameWithoutPath(str);
        if (File::fileExist(str))
        {
            return str;
        }
    }
    //str = File::findFileWithMainName(filename);
    //if (!isSubtitle(str))
    //{
    //    str = "";
    //}
    return str;
}

bool PotSubtitleManager::tryOpenSubtitle(const std::string& filename)
{
    bool b = false;
    return b;
}

bool PotSubtitleManager::isSubtitle(const std::string& filename)
{
    auto ext = convert::convertCase(File::getFileExt(filename));
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

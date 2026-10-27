#include "PotSubtitleManager.h"
#include "PotSubtitleAss.h"
#include "PotSubtitleSrt.h"
#include "filefunc.h"
#include "PotConv.h"
#include "strfunc.h"

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
    auto ext = strfunc::toLowerCase(filefunc::getFileExt(filename));
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
        if (!filename.empty())
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

    auto path = filename;
#ifndef _WINDLL
#ifdef _WIN32
    path = PotConv::conv(filename, "utf-8", "cp936");
#endif
#endif

    // Try matching subtitles beside the media file, then in its subs directory.
    for (auto& ext : ext_names_)
    {
        std::string str = "";
        str = filefunc::changeFileExt(path, ext);
        if (filefunc::fileExist(str))
        {
            return str;
        }
        auto parent = filefunc::getParentPath(str);
        if (parent.empty())
        {
            parent = ".";
        }
        str = parent + filefunc::get_path_char() + "subs" + filefunc::get_path_char() + filefunc::getFilenameWithoutPath(str);
        if (filefunc::fileExist(str))
        {
            return str;
        }
    }
    //str = filefunc::findFileWithMainName(filename);
    //if (!isSubtitle(str))
    //{
    //    str = "";
    //}
    return "";
}

bool PotSubtitleManager::tryOpenSubtitle(const std::string& filename)
{
    bool b = false;
    return b;
}

bool PotSubtitleManager::isSubtitle(const std::string& filename)
{
    auto ext = strfunc::toLowerCase(filefunc::getFileExt(filename));
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

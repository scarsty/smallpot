#include "Config.h"
#include "File.h"
#include "PotConv.h"
#include "Timer.h"
#include "convert.h"
#include <iostream>
#include <thread>

Config::Config()
{
    //init();
    ignore_strs_ =
    {
        ".bt.td",
        ".td",
    };
}

Config::~Config()
{
    //delete doc;
    //write();
}

void Config::init(std::string filepath)
{
    if (filepath != "")
    {
        filepath = filepath + "/";
    }
    filename_ = filepath + "smallpot.config.ini";
    printf("try find config file: %s\n", filename_.c_str());
    setString("filepath", filepath);
    ini_.loadFile(filename_);
    std::thread th{ [this]()
        { autoClearRecord(); } };
    th.detach();
}

void Config::write()
{
    ini_.saveFile(filename_);
}

std::string Config::getString(const std::string& name, std::string def /*= ""*/)
{
    if (!ini_.hasKey("", name))
    {
        setString(name, def);
    }
    return ini_.getString("", name);
}

int Config::getInteger(const std::string& name, int def /*= 0*/)
{
    if (!ini_.hasKey("", name))
    {
        setInteger(name, def);
    }
    return ini_.getInt("", name);
}

void Config::setString(const std::string& name, const std::string v)
{
    ini_.setKey("", name, v);
}

void Config::setInteger(const std::string& name, int v)
{
    setString(name, convert::formatString("%d", v));
}

int Config::getRecord(const std::string& name)
{
    auto s1 = getFileKey(name);
    for (auto& s : ini_.getAllKeys("record"))
    {
        if (getFileKey(s) == s1)
        {
            auto i = ini_.getInt("record", s);
            ini_.eraseKey("record", s);
            return i;
        }
    }
    return 0;
}

void Config::removeRecord(const std::string& name)
{
    auto mainname = dealFilename(name);
    ini_.eraseKey("record", mainname);
}

void Config::setRecord(const std::string& name, int v)
{
    ini_.setKey("record", dealFilename(name), std::to_string(v));
}

void Config::clearAllRecord()
{
    for (auto& s : ini_.getAllKeys("record"))
    {
        ini_.eraseKey("record", s);
    }
}

void Config::autoClearRecord()
{
    for (auto& s : ini_.getAllKeys("record"))
    {
        auto s1 = PotConv::conv(s, "utf-8", getString("sys_encode"));
        if (!File::fileExist(s1))
        {
            ini_.eraseKey("record", s);
        }
    }
}

std::string Config::dealFilename(const std::string& s0)
{
    auto s = s0;
    for (auto str : ignore_strs_)
    {
        s = convert::replaceAllSubString(s, str, "");
    }
    return s;
}

std::string Config::getFileKey(const std::string& s0)
{
    return File::getFileMainname(File::getFilenameWithoutPath(s0));
}

#include "Config.h"
#include "File.h"
#include "PotConv.h"
#include "Timer.h"
#include "convert.h"
#include <iostream>

Config::Config()
{
    //init();
    ignore_strs_ =
    {
        ".bt.td",
        ".td",
        " ",
        "_",
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
    ini_.loadFile(filename_);
}

void Config::write()
{
    ini_.saveFile(filename_);
}

std::string Config::getString(const std::string& name, std::string def /*= ""*/)
{
    return ini_.getString("", name);
}

int Config::getInteger(const std::string& name, int def /*= 0*/)
{
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
    return ini_.getInt("record", dealFilename(name));
}

void Config::removeRecord(const std::string& name)
{
    auto mainname = dealFilename(name);
    //
}

void Config::setRecord(const std::string& name, int v)
{
    ini_.setKey("record", dealFilename(name), std::to_string(v));
}

void Config::clearRecord()
{
    //if (record_)
    //{
    //    record_->DeleteChildren();
    //}
}

std::string Config::dealFilename(const std::string& s0)
{
    auto s = s0;
    //replaceAllString(s, " ", "_");
    s = File::getFilenameWithoutPath(s);
    for (auto str : ignore_strs_)
    {
        s = convert::replaceAllSubString(s, str, "");
    }
    s = File::getFileMainname(s);
    //s = PotConv::cp950toutf8(s);
    s = "_" + s;
    return s;
}

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
    ini_.loadFile(filename_);
    //setString("filepath", filepath);
    std::thread th{ [this]() { autoClearRecord(); return; } };
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
    return ini_.getInt("record", enStr(name));
}

std::string Config::getNewestRecord()
{
    auto rv = getSortedRecord();
    if (rv.empty())
    {
        return "";
    }
    return deStr(rv[0].filename);
}

void Config::removeRecord(const std::string& name)
{
    ini_.eraseKey("record", enStr(name));
}

void Config::setRecord(const std::string& name, int v)
{
    ini_.setKey("record", enStr(name), std::to_string(v) + "," + std::to_string(time(0)));
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
    auto rv = getSortedRecord();
    for (int i = 100; i < rv.size(); i++)
    {
        ini_.eraseKey("record", rv[i].filename);
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

std::string Config::enStr(const std::string& in)
{
    return in;
    std::string out;
    uint8_t a = 0;
    for (auto& c : in)
    {
        a += c;
    }
    out += convert::formatString("%02x", a);
    for (auto& c : in)
    {
        out += convert::formatString("%02x", uint8_t(c) ^ a);
    }
    std::reverse(out.begin(), out.end());
    return out;
}

std::string Config::deStr(std::string out)
{
    return out;
    std::string in;
    std::reverse(out.begin(), out.end());

    auto hex2c = [](std::string hex)
    {
        uint8_t res = 0;
        for (auto c : hex)
        {
            uint8_t r = 0;
            if (c >= '0' && c <= '9')
            {
                r = c - '0';
            }
            if (c >= 'a' && c <= 'f')
            {
                r = c - 'a' + 10;
            }
            if (c >= 'A' && c <= 'F')
            {
                r = c - 'A' + 10;
            }
            res = res * 16 + r;
        }
        return res;
    };

    uint8_t a = hex2c(out.substr(0, 2));

    for (int i = 2; i < out.size(); i += 2)
    {
        auto c = hex2c(out.substr(i, 2)) ^ a;
        in += c;
    }
    return in;
}

std::vector<Config::Record> Config::getSortedRecord()
{
    std::vector<Record> rv;
    for (auto& s : ini_.getAllKeys("record"))
    {
        auto s1 = PotConv::conv(deStr(s), "utf-8", getString("sys_encode"));
        if (!File::fileExist(s1))
        {
            ini_.eraseKey("record", s);
        }
        else
        {
            Record r;
            r.filename = s;
            auto v = convert::findNumbers<int64_t>(ini_.getString("record", s));
            if (v.size() >= 2)
            {
                r.time = v[1];
            }
            rv.push_back(r);
        }
    }
    std::sort(rv.begin(), rv.end(), [](const Record& l, const Record& r) { return l.time > r.time; });
    return rv;
}

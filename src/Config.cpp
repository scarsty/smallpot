﻿#include "Config.h"
#include "PotConv.h"
#include "Timer.h"
#include "filefunc.h"
#include "strfunc.h"
#include <algorithm>
#include <iostream>
#include <print>
#include <thread>

Config::Config()
{
    //init();
    ignore_strs_ = {
        ".bt.td",
        ".td",
    };
    std::print("Config init\n");
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
    filename_ = filepath + "smallpot.config.json";
    std::print("try find config file: {}\n", filename_);
    parse(filefunc::readFileToString(filename_));
    std::print("read config file\n");
}

void Config::write()
{
    filefunc::writeStringToFile(allToString(), filename_);
    std::print("write config file\n");
}

int Config::getRecord(const std::string& name)
{
    return (*this)["record"][enStr(name)]["progress"].toInt();
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
    (*this)["record"].erase(enStr(name));
}

void Config::setRecord(const std::string& name, int v)
{
    (*this)["record"][enStr(name)]["progress"] = v;
    (*this)["record"][enStr(name)]["time"] = Timer::getNowAsString();
}

void Config::clearAllRecord()
{
    (*this)["record"].clear();
}

//
void Config::autoClearRecord()
{
    auto rv = getSortedRecord();
    if (!rv.empty())
    {
        if (Timer::getNowAsString().substr(0, 10) == rv[0].time.substr(0, 10))
        {
            return;    //一天只清一次
        }
    }
    int i = 0;
    for (auto& r : rv)
    {
        if (i > 100)
        {
            (*this)["record"].erase(rv[i].filename);
        }
        else
        {
            auto s1 = PotConv::conv(deStr(r.filename), "utf-8", (*this)["sys_encode"].toString());
            if (!filefunc::fileExist(s1))
            {
                (*this)["record"].erase(r.filename);
            }
        }
    }
}

//std::string Config::dealFilename(const std::string& s0)
//{
//    auto s = s0;
//    for (auto str : ignore_strs_)
//    {
//        s = strfunc::replaceAllSubString(s, str, "");
//    }
//    return s;
//}
//
std::string Config::enStr(const std::string& in)
{
    return in;
    std::string out;
    uint8_t a = 0;
    for (auto& c : in)
    {
        a += c;
    }
    out += std::format("{:02x}", a);
    for (auto& c : in)
    {
        out += std::format("{:02x}", uint8_t(c) ^ a);
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
    for (auto [k, v] : (*this)["record"].asMap())
    {
        Record r;
        uint64_t t;
        r.filename = k;
        r.second = v["progress"].toInt();
        r.time = v["time"].toString();
        rv.push_back(r);
    }
    std::sort(rv.begin(), rv.end(), [](const Record& l, const Record& r)
        {
            return l.time > r.time;
        });
    return rv;
}

std::string Config::findSuitableFilename(const std::string& filename)
{
    //若试图打开的文件实际是一个目录，此处将其改为文件夹下最近打开过的文件
    auto filename1 = filename;
    for (auto& rv : getSortedRecord())
    {
        if (rv.filename == filename)
        {
            return filename;
        }
        if (filefunc::pathExist(filename) && rv.filename.find(filename) == 0)
        {
            filename1 = rv.filename;
            return filename1;
        }
    }
    return filename1;
}

#include "Config.h"
#include "File.h"
#include "Timer.h"
#include "libconvert.h"
#include <iostream>

Config Config::config_;

Config::Config()
{
    //init();
    ignore_strs =
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

void Config::init(const std::string& filepath)
{
    filename_ = filepath + "/tinypot-config.xml";
    printf("try find config file: %s\n", filename_.c_str());
    doc_.LoadFile(filename_.c_str());
    //初始化结构
    if (doc_.Error())
    {
        //_doc.DeleteChildren();
        doc_.LinkEndChild(doc_.NewDeclaration());
        root_ = doc_.NewElement("root");
    }
    else
    {
        root_ = doc_.FirstChildElement("root");
    }

    record_ = root_->FirstChildElement("record");
    if (!record_)
    {
        record_ = root_->InsertFirstChild(doc_.NewElement("record"))->ToElement();
    }

    auto setInit = [&](const char* s)
    {
        if (getInteger(s) == 0)
        {
            setInteger(0, s);
        }
    };
    setInit("record_name");
    setInit("auto_play_recent");

    //auto perAttr = record_->FirstChildElement();
    //while (perAttr)
    //{
    //    std::cout << perAttr->Name() << ":" << perAttr->FirstChildElement("date")->GetText() << std::endl;
    //    perAttr = perAttr->NextSiblingElement();
    //}
}

void Config::write()
{
    //_doc.LinkEndChild(_doc.NewDeclaration());
    doc_.LinkEndChild(root_);
    doc_.SaveFile(filename_.c_str());
}

tinyxml2::XMLElement* Config::getElement(tinyxml2::XMLElement* parent, const char* name)
{
    auto p = parent->FirstChildElement(name);
    if (p)
    {
        return p;
    }
    else
    {
        p = parent->InsertFirstChild(doc_.NewElement(name))->ToElement();
        p->SetText("");
        return p;
    }
}

int Config::getRecord(const char* name)
{
    if (strlen(name) == 0)
    {
        return 0;
    }
    std::string key = dealFilename(name);
    auto r = getElement(record_, key.c_str());
    r = getElement(r, "time");
    const char* str = r->GetText();
    if (!str)
    {
        return 0;
    }
    return atoi(str);
}

void Config::removeRecord(const char* name)
{
    if (strlen(name) == 0)
    {
        return;
    }
    auto mainname = dealFilename(name);
    record_->DeleteChild(getElement(record_, mainname.c_str()));
}

void Config::setRecord(int v, const char* name)
{
    if (strlen(name) == 0)
    {
        return;
    }
    std::string key = dealFilename(name);
    auto r = getElement(record_, key.c_str());
    getElement(r, "time")->SetText(convert::formatString("%d", v).c_str());
    getElement(r, "date")->SetText(Timer::getNowAsString("%F %T").c_str());
    if (getInteger("record_name"))
    {
        getElement(r, "name")->SetText(name);
    }
    //r->SetText(File::formatString("%d", v).c_str());
}

void Config::clearRecord()
{
    if (record_)
    {
        record_->DeleteChildren();
    }
}

std::string Config::getString(const char* name, std::string def /*= ""*/)
{
    auto p = root_->FirstChildElement(name);
    if (p && p->FirstChild())
    {
        return p->GetText();
    }
    else
    {
        return def;
    }
}

int Config::getInteger(const char* name, int def /*= 0*/)
{
    return atoi(getString(name, convert::formatString("%d", def)).c_str());
}

double Config::getDouble(const char* name, double def /*= 0.0*/)
{
    return atof(getString(name, convert::formatString("%f", def)).c_str());
}

bool Config::getBool(bool& v, const char* name)
{
    return atoi(getString(name, "0").c_str()) != 0;
}

void Config::setString(const std::string v, const char* name)
{
    getElement(root_, name)->SetText(v.c_str());
}

void Config::setInteger(int v, const char* name)
{
    setString(convert::formatString("%d", v), name);
}

void Config::setDouble(double v, const char* name)
{
    setString(convert::formatString("%f", v), name);
}

void Config::setBool(bool v, const char* name)
{
    setString(convert::formatString("%d", v != 0), name);
}

int Config::replaceAllString(std::string& s, const std::string& oldstring, const std::string& newstring)
{
    int pos = s.find(oldstring);
    while (pos >= 0)
    {
        s.erase(pos, oldstring.length());
        s.insert(pos, newstring);
        pos = s.find(oldstring, pos + newstring.length());
    }
    return pos + newstring.length();
}

std::string Config::dealFilename(const std::string& s0)
{
    auto s = s0;
    //replaceAllString(s, " ", "_");
    s = File::getFilenameWithoutPath(s);
    for (auto str : ignore_strs)
    {
        replaceAllString(s, str, "");
    }
    s = File::getFileMainname(s);
    //s = PotConv::cp950toutf8(s);
    s = sha3_(s).substr(0, 10);
    s = "_" + s;
    return s;
}

#include "Config.h"
#include "File.h"
#include "PotConv.h"
#include "Timer.h"
#include "libconvert.h"
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
    filename_ = filepath + "tinypot-config.xml";
    printf("try find config file: %s\n", filename_.c_str());
    doc_.LoadFile(filename_.c_str());
    //初始化结构
    if (doc_.Error())
    {
        //_doc.DeleteChildren();
        doc_.LinkEndChild(doc_.NewDeclaration());
        root_ = doc_.NewElement("root");
        setInteger("auto_play_recent", 0);
        setInteger("record_name", 0);
    }
    else
    {
        root_ = doc_.FirstChildElement("root");
    }
    record_ = getElement(root_, "record");
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

std::string Config::getString(const std::string& name, std::string def /*= ""*/)
{
    auto p = root_->FirstChildElement(name.c_str());
    if (p && p->FirstChild())
    {
        return p->GetText();
    }
    else
    {
        return def;
    }
}

int Config::getInteger(const std::string& name, int def /*= 0*/)
{
    return atoi(getString(name, convert::formatString("%d", def)).c_str());
}

void Config::setString(const std::string& name, const std::string v)
{
    getElement(root_, name.c_str())->SetText(v.c_str());
}

void Config::setInteger(const std::string& name, int v)
{
    setString(name, convert::formatString("%d", v));
}

int Config::getRecord(const std::string& name)
{
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

void Config::removeRecord(const std::string& name)
{
    auto mainname = dealFilename(name);
    record_->DeleteChild(getElement(record_, mainname.c_str()));
}

void Config::setRecord(const std::string& name, int v)
{
    std::string key = dealFilename(name);
    auto r = getElement(record_, key.c_str());
    getElement(r, "time")->SetText(convert::formatString("%d", v).c_str());
    getElement(r, "date")->SetText(Timer::getNowAsString("%F %T").c_str());
    if (getInteger("record_name"))
    {
        getElement(r, "name")->SetText(name.c_str());
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

std::string Config::dealFilename(const std::string& s0)
{
    auto s = s0;
    //replaceAllString(s, " ", "_");
    s = File::getFilenameWithoutPath(s);
    for (auto str : ignore_strs_)
    {
        convert::replaceAllString(s, str, "");
    }
    s = File::getFileMainname(s);
    //s = PotConv::cp950toutf8(s);
    s = sha3_(s).substr(0, 10);
    s = "_" + s;
    return s;
}

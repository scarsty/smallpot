#include "BigPotConfig.h"

BigPotConfig BigPotConfig::_config;

BigPotConfig::BigPotConfig()
{
    _this = &_config;
    //init();
}


BigPotConfig::~BigPotConfig()
{
    //delete doc;
    //write();
}

void BigPotConfig::init(const std::string& filepath)
{
    _filename = filepath + "/config.xml";
    printf("try find config file: %s\n", _filename.c_str());
    _doc.LoadFile(_filename.c_str());
#ifdef _DEBUG
    //_doc.Print();
#endif
    //初始化结构
    if (_doc.Error())
    {
        //_doc.DeleteChildren();
        _doc.LinkEndChild(_doc.NewDeclaration());
        _root = _doc.NewElement("root");
    }
    else
    {
        _root = _doc.FirstChildElement("root");
    }

    _record = _root->FirstChildElement("record");
    if (!_record)
    { _record = _root->InsertFirstChild(_doc.NewElement("record"))->ToElement(); }
}

void BigPotConfig::write()
{
    //_doc.LinkEndChild(_doc.NewDeclaration());
    _doc.LinkEndChild(_root);
    _doc.SaveFile(_filename.c_str());
}

tinyxml2::XMLElement* BigPotConfig::getElement(tinyxml2::XMLElement* parent, const char* name)
{
    auto p = parent->FirstChildElement(name);
    if (p)
    {
        return p;
    }
    else
    {
        p = parent->InsertFirstChild(_doc.NewElement(name))->ToElement();
        p->SetText("");
        return p;
    }
}

int BigPotConfig::getRecord(const char* name)
{
    if (strlen(name) == 0) { return 0; }
    auto mainname = BigPotString::getFilenameWithoutPath(name);
    const char* str = getElement(_record, ("_" + _sha3(mainname)).c_str())->GetText();
    if (!str)
    { return 0; }
    return atoi(str);
}

void BigPotConfig::removeRecord(const char* name)
{
    if (strlen(name) == 0) { return; }
    auto mainname = BigPotString::getFilenameWithoutPath(name);
    _record->DeleteChild(getElement(_record, ("_" + _sha3(mainname)).c_str()));
}

void BigPotConfig::setRecord(int v, const char* name)
{
    if (strlen(name) == 0) { return; }
    auto mainname = BigPotString::getFilenameWithoutPath(name);
    getElement(_record, ("_" + _sha3(mainname)).c_str())
    ->SetText(BigPotString::formatString("%d", v).c_str());
}

void BigPotConfig::clearRecord()
{
    if (_record)
    {
        _record->DeleteChildren();
    }
}

std::string BigPotConfig::getString(const char* name, std::string def /*= ""*/)
{
    auto p = _root->FirstChildElement(name);
    if (p && p->FirstChild())
    {
        return p->GetText();
    }
    else
    {
        return def;
    }
}

int BigPotConfig::getInteger(const char* name, int def /*= 0*/)
{
    return atoi(getString(name, BigPotString::formatString("%d", def)).c_str());
}

double BigPotConfig::getDouble(const char* name, double def /*= 0.0*/)
{
    return atof(getString(name, BigPotString::formatString("%f", def)).c_str());
}

bool BigPotConfig::getBool(bool& v, const char* name)
{
    return atoi(getString(name, "0").c_str()) != 0;
}

void BigPotConfig::setString(const std::string v, const char* name)
{
    getElement(_root, name)->SetText(v.c_str());
}

void BigPotConfig::setInteger(int v, const char* name)
{
    setString(BigPotString::formatString("%d", v), name);
}

void BigPotConfig::setDouble(double v, const char* name)
{
    setString(BigPotString::formatString("%f", v), name);
}

void BigPotConfig::setBool(bool v, const char* name)
{
    setString(BigPotString::formatString("%d", v != 0), name);
}


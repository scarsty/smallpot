#pragma once

#include "tinyxml2/tinyxml2.h"
#include <stdint.h>
#include "hash/sha3.h"

class Config
{
private:
    static Config config_;
    std::string content_;
    std::string filename_;
    SHA3 sha3_;
    Config();
    virtual ~Config();

    tinyxml2::XMLDocument doc_;
    tinyxml2::XMLElement* root_, *record_;

    tinyxml2::XMLElement* getElement(tinyxml2::XMLElement* parent, const char* name);

public:
    void init(const std::string& filepath);
    void write();
    static Config* getInstance() { return &config_; };

    //xml只有字串，故首先完成字串功能
    std::string getString(const char* name, std::string def = "");
    int getInteger(const char* name, int def = 0);
    double getDouble(const char* name, double def = 0.0);
    bool getBool(bool& v, const char* name);

    void setString(const std::string v, const char* name);
    void setInteger(int v, const char* name);
    void setDouble(double v, const char* name);
    void setBool(bool v, const char* name);

    //记录
    int getRecord(const char* name);
    void removeRecord(const char* name);
    void setRecord(int v, const char* name);
    void clearRecord(); //string replace(string str, const char *string_to_replace, const char *new_string);

    int replaceAllString(std::string& s, const std::string& oldstring, const std::string& newstring);

    int dealFilename(std::string& s);

};


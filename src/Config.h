#pragma once

#include "hash/sha3.h"
#include "tinyxml2/tinyxml2.h"
#include <stdint.h>
#include <vector>

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
    tinyxml2::XMLElement *root_, *record_;

    tinyxml2::XMLElement* getElement(tinyxml2::XMLElement* parent, const char* name);

    std::vector<std::string> ignore_strs_;

public:
    void init(const std::string& filepath);

    void write();
    static Config* getInstance() { return &config_; };

    //xml只有字串，故首先完成字串功能
    std::string getString(const std::string& name, std::string def = "");
    int getInteger(const std::string& name, int def = 0);

    void setString(const std::string& name, const std::string v);
    void setInteger(const std::string& name, int v);

    //记录
    int getRecord(const std::string& name);
    void removeRecord(const std::string& name);
    void setRecord(const std::string& name, int v);
    void clearRecord();

    std::string dealFilename(const std::string& s0);
};

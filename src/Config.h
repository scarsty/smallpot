#pragma once

#include "INIReader.h"
#include <stdint.h>
#include <vector>

class Config
{
private:
    std::string content_;
    std::string filename_;
    Config();
    virtual ~Config();

    INIReaderNormal ini_;

    std::vector<std::string> ignore_strs_;

public:
    void init(std::string filepath);

    void write();
    static Config* getInstance()
    {
        static Config c;
        return &c;
    }

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

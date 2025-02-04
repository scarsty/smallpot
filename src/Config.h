#pragma once

#include "FakeJson.h"
#include <vector>

class Config : public FakeJson
{
    struct Record
    {
        std::string filename, path;
        int second;
        std::string time;
    };

    //
private:
    std::string content_;
    std::string filename_;
    Config();
    virtual ~Config();
    //
    //    INIReaderNormal ini_;
    //
    std::vector<std::string> ignore_strs_;
    //
public:
    void init(std::string filepath);
    //
    void write();

    static Config& getInstance()
    {
        static Config c;
        return c;
    }

    template <typename K, typename T>
    T get(const K& k, T default_v)
    {
        if (exist(k))
        {
            return (*this)[k].to<T>();
        }
        else
        {
            return default_v;
        }
    }

    //记录
    int getRecord(const std::string& name);
    std::string getNewestRecord();
    void removeRecord(const std::string& name);
    void setRecord(const std::string& name, int v);
    void clearAllRecord();
    void autoClearRecord();

    //    std::string dealFilename(const std::string& s0);

    std::string enStr(const std::string& in);
    std::string deStr(std::string out);
    std::vector<Record> getSortedRecord();

    std::string findSuitableFilename(const std::string& filename);
};

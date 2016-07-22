//变量名规则
//私有带前下划线
//保护带后下划线
//通常不使用公共变量而通过方法来读写

#pragma once

#include "Engine.h"

#ifdef _WIN32
#define _path_  ('\\')
#else
#define _path_  ('/')
#endif


//可能不安全

class BigPotBase
{
protected:
    Engine* engine_;
    class BigPotConfig* config_;
public:
    BigPotBase();
    ~BigPotBase() {};

    //void setFilePath(char *s) { BigPotString::setFilePath(s); }
    //static bool fileExist(const string& filename);
    //void safedelete(void* p){ if (p) delete p; p = nullptr; };
    enum FindMode
    {
        FINDLAST = 0,
        FINDFIRST = 1
    };

    static bool fileExist(const std::string& filename);
    static std::string readStringFromFile(const std::string& filename);
    static std::string getFileExt(const std::string& filename);
    static std::string getFileMainname(const std::string& fileName, FindMode mode = FINDLAST);
    static std::string getFilenameWithoutPath(const std::string& fileName);
    static std::string changeFileExt(const std::string& filename, const std::string& ext);
    static std::string getFilePath(const std::string& filename);

    static std::string fingFileWithMainName(const std::string& filename);
    static std::string toLowerCase(const std::string& str);
    static std::string formatString(const char* format, ...);

};


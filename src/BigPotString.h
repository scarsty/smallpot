#pragma once

#include <iostream>
#include <fstream>
#ifndef _MSC_VER
#include <sys/uio.h>
#else
#include <io.h>
#endif

#include <stdio.h>
#include <stdarg.h>

//主要是字串和文件名处理，随时可能使用

#ifdef _WIN32
#define _path_  ('\\')
#else
#define _path_  ('/')
#endif

namespace BigPotString
{
    /*private:
        BigPotString();
        virtual ~BigPotString();
        static string _filepath;
        public:*/
    enum FindMode
    {
        FINDLAST = 0,
        FINDFIRST = 1
    };

    bool fileExist(const std::string& filename);
    std::string readStringFromFile(const std::string& filename);
    std::string getFileExt(const std::string& filename);
    std::string getFileMainname(const std::string& fileName, FindMode mode = FINDLAST);
    std::string getFilenameWithoutPath(const std::string& fileName);
    std::string changeFileExt(const std::string& filename, const std::string& ext);
    std::string getFilePath(const std::string& filename);

    std::string fingFileWithMainName(const std::string& filename);
    std::string toLowerCase(const std::string& str);
    std::string formatString(const char* format, ...);

    //void setFilePath(char *s);
    //std::string getFilePath();

}
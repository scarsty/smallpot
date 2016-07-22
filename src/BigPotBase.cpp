#include "BigPotBase.h"
#include "BigPotConfig.h"

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

BigPotBase::BigPotBase()
{
    engine_ = Engine::getInstance();
    config_ = BigPotConfig::getInstance();
}

bool BigPotBase::fileExist(const std::string& filename)
{
    if (filename.length() <= 0)
    { return false; }

    std::fstream file;
    bool ret = false;
    file.open(filename.c_str(), std::ios::in);
    if (file)
    {
        ret = true;
        file.close();
    }
    return ret;
}

std::string BigPotBase::readStringFromFile(const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "rb");
    if (fp == nullptr)
    {
        printf("Can not open file %s\n", filename.c_str());
        return "";
    }
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, 0);
    char* s = new char[length + 1];
    for (int i = 0; i <= length; s[i++] = '\0');
    fread(s, length, 1, fp);
    std::string str(s);
    fclose(fp);
    delete[] s;
    return str;
}

std::string BigPotBase::getFileExt(const std::string& filename)
{
    int pos_p = filename.find_last_of(_path_);
    int pos_d = filename.find_last_of('.');
    if (pos_p < pos_d)
    { return filename.substr(pos_d + 1); }
    return "";
}

//一般是找最后一个点，为1是找第一个点
std::string BigPotBase::getFileMainname(const std::string& filename, FindMode mode)
{
    int pos_p = filename.find_last_of(_path_);
    int pos_d = filename.find_last_of('.');
    if (mode == FINDFIRST)
    { pos_d = filename.find_first_of('.', pos_p + 1); }
    if (pos_p < pos_d)
    { return filename.substr(0, pos_d); }
    return filename;
}

std::string BigPotBase::changeFileExt(const std::string& filename, const std::string& ext)
{
    auto e = ext;
    if (e != "" && e[0] != '.')
    { e = "." + e; }
    return getFileMainname(filename) + e;
}

std::string BigPotBase::getFilePath(const std::string& filename)
{
    int pos_p = filename.find_last_of(_path_);
    if (pos_p != std::string::npos)
    { return filename.substr(0, pos_p); }
    return "";
}

//查找相似文件名的文件，只找两个
std::string BigPotBase::fingFileWithMainName(const std::string& filename)
{
#ifndef _MSC_VER
    return "";
#else
    _finddata_t file;
    long fileHandle;
    std::string path = getFilePath(filename);
    std::string ext = getFileExt(filename);
    if (path != "") { path = path + _path_; }
    std::string findname = getFileMainname(filename) + ".*";
    std::string ret = "";
    fileHandle = _findfirst(findname.c_str(), &file);
    ret = path + file.name;
    if (getFileExt(ret) == ext)
    {
        _findnext(fileHandle, &file);
        ret = path + file.name;
        if (getFileExt(ret) == ext)
        { ret = ""; }
    }
    _findclose(fileHandle);
    return ret;
#endif
}

std::string BigPotBase::toLowerCase(const std::string& str)
{
    auto str1 = str;
    std::transform(str1.begin(), str1.end(), str1.begin(), tolower);
    return str1;
}

std::string BigPotBase::formatString(const char* format, ...)
{
    char s[4096];
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(s, sizeof(s), format, arg_ptr);
    va_end(arg_ptr);
    return s;
}

std::string BigPotBase::getFilenameWithoutPath(const std::string& filename)
{
    int pos_p = filename.find_last_of(_path_);
    if (pos_p != std::string::npos)
    { return filename.substr(pos_p + 1); }
    return filename;
}
#pragma once
#include <cstring>
#include <string>
#include <vector>

namespace filefunc
{

//read and write file
bool fileExist(const std::string& name);
bool pathExist(const std::string& name);

std::vector<char> readFile(const std::string& filename, int length = -1);
int writeFile(const char* data, int length, const std::string& filename);
int writeFile(const std::vector<char>& data, const std::string& filename);

std::string readFileToString(const std::string& filename);
int writeStringToFile(const std::string& str, const std::string& filename);

template <class T>
void readDataToVector(const char* data, int length, std::vector<T>& v, int length_one)
{
    int count = length / length_one;
    v.resize(count);
    for (int i = 0; i < count; i++)
    {
        memcpy(&v[i], data + length_one * i, length_one);
    }
}

template <class T>
void readDataToVector(const char* data, int length, std::vector<T>& v)
{
    readDataToVector(data, length, v, sizeof(T));
}

template <class T>
void readFileToVector(const std::string& filename, std::vector<T>& v)
{
    auto buffer = readFile(filename);
    readDataToVector(buffer.data(), buffer.size(), v);
}

template <class T>
void writeVectorToData(char* data, int length, std::vector<T>& v, int length_one)
{
    int count = length / length_one;
    v.resize(count);
    for (int i = 0; i < count; i++)
    {
        memcpy(data + length_one * i, &v[i], length_one);
    }
}

template <class T>
int writeVectorToFile(const std::vector<T>& v, const std::string& filename)
{
    return writeFile((const char*)v.data(), v.size() * sizeof(T), filename);
}

//other file operations

bool is_path_char(char c);
char get_path_char();
size_t getLastPathCharPos(const std::string& filename, int utf8 = 0);
size_t getLastEftPathCharPos(const std::string& filename, int utf8 = 0);

std::vector<std::string> getFilesInPath(const std::string& pathname, int recursive = 0, int include_path = 0, int absolute_path = 0);
std::string getFileTime(const std::string& filename, const std::string& format = "%Y-%m-%d %H:%M:%S");
void changePath(const std::string& path);
std::string getCurrentPath();
void makePath(const std::string& path);
void copyFile(const std::string& src, const std::string& dst);
void moveFile(const std::string& src, const std::string& dst);
void removeFile(const std::string& filename);
void removePath(const std::string& path);
std::string getRelativePath(const std::string& filename, const std::string& basepath);

//functions about file name
std::string getFileExt(const std::string& filename);
std::string getFileMainName(const std::string& filename);
std::string getFilenameWithoutPath(const std::string& filename);
std::string getFileMainNameWithoutPath(const std::string& filename);
std::string changeFileExt(const std::string& filename, const std::string& ext);
std::string getParentPath(const std::string& filename, int utf8 = 0);    //utf8 has no effect on non-win32
std::string getFilePath(const std::string& filename, int utf8 = 0);
std::string toLegalFilename(const std::string& filename, int allow_path = 1);
std::string getAbsolutePath(const std::string& filename);
bool compareNature(const std::string& a, const std::string& b);

}    //namespace filefunc

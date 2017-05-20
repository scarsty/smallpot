#pragma once
#include <string>

class File
{
public:
    File();
    virtual ~File();

    enum FindMode
    {
        FINDLAST = 0,
        FINDFIRST = 1,
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
    static void changePath(const std::string& path);

	static int replaceAllString(std::string& s, const std::string& oldstring, const std::string& newstring);
private:
    static int getLastPathPos(const std::string& filename);
};


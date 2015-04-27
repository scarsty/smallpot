#pragma once

#include <iostream>
#include <fstream>
#ifdef __APPLE__
#include <sys/uio.h>
#else
#include <io.h>
#endif

#include <stdio.h>
#include <stdarg.h> 

using namespace std;

//主要是字串和文件名处理，随时可能使用

#ifdef _MSC_VER
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

	bool fileExist(const string& filename);
	string readStringFromFile(const string& filename);
	string getFileExt(const string& filename);
	string getFileMainname(const string& fileName, FindMode mode = FINDLAST);
	string getFilenameWithoutPath(const string& fileName);
	string changeFileExt(const string& filename, const string& ext);
	string getFilePath(const string& filename);

	string fingFileWithMainName(const string& filename);
	string toLowerCase(const string& str);
	string formatString(const char *format, ...);

	void setFilePath(char *s);
	string getFilePath();

}
//变量名规则
//私有带前下划线
//保护带后下划线
//通常不使用公共变量而通过方法来读写

#pragma once

#include "BigPotEngine.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

//公共类
//可能不安全

class BigPotBase
{
protected:
	static string filepath_;
	BigPotEngine* engine_;
private:
#ifdef WIN32
	char _path_ = '\\';
#else
	char _path_ = '/';
#endif
public:
	BigPotBase() { engine_ = BigPotEngine::getInstance(); };
	~BigPotBase() {};
	void safedelete(void* p){ if(p) delete p; p = nullptr; };
	bool fileExist(const string& filename);
	string readStringFromFile(const string& filename);
	string getFileExt(const string& filename);
	string getFileWithoutExt(const string& fileName);
	string changeFileExt(const string& filename, const string& ext);
	string getFilePath(const string& filename);
	void setFilePath(char *s) { filepath_ = getFilePath(s); }
};


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
	BigPotEngine* engine_;
public:
	BigPotBase() { engine_ = BigPotEngine::getInstance(); };
	~BigPotBase() {};
	void safedelete(void* p){ if(p) delete p; p = nullptr; };
	bool fileexist(const string& filename);
	string readStringFromFile(const string& filename);
};


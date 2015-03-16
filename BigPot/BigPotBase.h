//变量名规则
//私有带前下划线
//保护带后下划线
//通常不使用公共变量而通过方法来读写

#pragma once

#include "BigPotEngine.h"
#include "BigPotString.h"
#include "BigPotConfig.h"


using namespace std;


//可能不安全

class BigPotBase
{
protected:
	BigPotEngine* engine_;
	BigPotConfig* config_;
public:
	BigPotBase();
	~BigPotBase() {};

	//void setFilePath(char *s) { BigPotString::setFilePath(s); }
	//static bool fileExist(const string& filename);
	//void safedelete(void* p){ if (p) delete p; p = nullptr; };

};


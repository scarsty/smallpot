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
	BigPotEngine* engine;
public:
	BigPotBase() { engine = BigPotEngine::getInstance(); };
	~BigPotBase() {};
	void safedelete(void* p){ if(p) delete p; p = nullptr; };
	bool fileexist(const string& filename);
	string readStringFromFile(const string& filename);
};


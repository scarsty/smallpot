#pragma once

#include "BigPotControl.h"
#include <string>
#include <iostream>

using namespace std;

//公共类
//可能不安全

class BigPotBase
{
protected:
	BigPotControl* control;
public:
	BigPotBase() { control = BigPotControl::getInstance(); };
	~BigPotBase() {};
};


//变量名规则
//私有带前下划线
//保护带后下划线
//通常不使用公共变量而通过方法来读写

#pragma once

#include "Engine.h"

//可能不安全

class PotBase
{
protected:
    Engine* engine_;

public:
    PotBase();
    ~PotBase() {};

    //void setFilePath(char *s) { BigPotString::setFilePath(s); }
    //static bool fileExist(const string& filename);
    //void safedelete(void* p){ if (p) delete p; p = nullptr; };
};

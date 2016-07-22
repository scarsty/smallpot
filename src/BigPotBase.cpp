#include "BigPotBase.h"

//主要是字串和文件名处理，随时可能使用

BigPotBase::BigPotBase()
{
    engine_ = Engine::getInstance();
    config_ = Config::getInstance();
}


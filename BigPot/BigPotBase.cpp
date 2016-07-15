#include "BigPotBase.h"

BigPotBase::BigPotBase()
{
    engine_ = Engine::getInstance();
    config_ = BigPotConfig::getInstance();
}


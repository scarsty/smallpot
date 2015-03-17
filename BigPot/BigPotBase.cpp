#include "BigPotBase.h"

BigPotBase::BigPotBase()
{
	engine_ = BigPotEngine::getInstance();
	config_ = BigPotConfig::getInstance();
}


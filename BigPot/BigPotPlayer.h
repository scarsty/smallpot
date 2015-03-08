#pragma once

#include "BigPotBase.h"
#include "BigPotMedia.h"
#include "BigPotUI.h"
#include "BigPotConv.h"
#include "BigPotConfig.h"

class BigPotPlayer : public BigPotBase
{
public:
	BigPotPlayer();
	virtual ~BigPotPlayer();
private:
	string filename;
	BigPotMedia* media = nullptr;
	BigPotUI* UI = nullptr;
	BigPotConfig *config = nullptr;
	int w, h;
	bool run = true;

	string sys_encode = "cp936", BP_encode = "utf-8";
	int volume;
	int eventLoop();

	int drawTex2();
	int showTex2();
public:
	int beginWithFile(const string &filename);
	
};




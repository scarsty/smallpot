#pragma once

#include "BigPotBase.h"
#include "BigPotMedia.h"
#include "BigPotUI.h"
#include "BigPotConv.h"

class BigPotPlayer : public BigPotBase
{
public:
	BigPotPlayer();
	virtual ~BigPotPlayer();
private:
	string filename;
	BigPotMedia* media = nullptr;
	BigPotUI* UI = nullptr;
	int w, h;

	int init();
	int createScreenTexture();

	int eventLoop();

	int drawTex2();
	int showTex2();
public:
	int playFile(const string &filename);
	
};




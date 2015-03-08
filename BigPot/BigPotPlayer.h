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
	string drop_filename = "";
	int cur_time = 0;
	BigPotMedia* media = nullptr;
	BigPotUI* UI = nullptr;
	BigPotConfig *config = nullptr;
	int w, h;
	bool run = true;

	string sys_encode = "cp936", BP_encode = "utf-8";
	int cur_volume;
	int eventLoop();

	int drawTex2();
	int showTex2();
	string getSysString(const string& str);
	int getFileTime(const string& filename);
	int setFileTime(int time, const string& filename);
public:
	int beginWithFile(const string &filename);
	
};




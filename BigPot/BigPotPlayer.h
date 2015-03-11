#pragma once

#include "BigPotBase.h"
#include "BigPotMedia.h"
#include "BigPotUI.h"
#include "BigPotConv.h"
#include "BigPotConfig.h"
#include "BigPotSubtitle.h"

class BigPotPlayer : public BigPotBase
{
public:
	BigPotPlayer();
	virtual ~BigPotPlayer();
private:
	string _drop_filename = "";
	int _cur_time = 0;

	BigPotMedia* _media = nullptr;
	BigPotUI* _UI = nullptr;
	BigPotConfig *_config = nullptr;
	BigPotSubtitle *_subtitle;

	int _w, _h;
	bool _run = true;

	string _sys_encode = "cp936", _BP_encode = "utf-8";
	int _cur_volume;
	int eventLoop();

	int drawTex2();
	int showTex2();
	string getSysString(const string& str);
	int getRecordFileTime(const string& filename);
	int setRecordFileTime(int time, const string& filename);
public:
	int beginWithFile(const string &filename);
	
};




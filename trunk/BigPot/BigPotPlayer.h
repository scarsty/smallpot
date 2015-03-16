#pragma once

#include "BigPotBase.h"
#include "BigPotMedia.h"
#include "BigPotUI.h"
#include "BigPotConv.h"
#include "BigPotConfig.h"
#include "BigPotSubtitleFactory.h"

class BigPotPlayer : public BigPotBase
{
public:
	BigPotPlayer();
	BigPotPlayer(char* s) : BigPotPlayer() { _filepath = BigPotString::getFilePath(s); }
	virtual ~BigPotPlayer();
private:
	string _drop_filename = "", _filepath;
	int _cur_time = 0;

	BigPotMedia* _media = nullptr;
	BigPotUI* _UI = nullptr;
	BigPotSubtitle *_subtitle = nullptr;
	BigPotSubtitleFactory *_subtitle_factory = nullptr;

	int _w, _h;
	bool _run = true;

	string _sys_encode = "cp936", _BP_encode = "utf-8";
	int _cur_volume;
	int eventLoop();

	//int drawTex2();
	//int showTex2();
	//string getSysString(const string& str);
public:
	int beginWithFile(const string &filename);
	int init();
	void destroy();
	void openMedia(const string& filename);
	void closeMedia(const string& filename);
	
};




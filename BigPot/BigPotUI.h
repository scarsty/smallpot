#pragma once
#include "BigPotBase.h"


class BigPotUI : public BigPotBase
{
public:
	BigPotUI();
	virtual ~BigPotUI();

	BP_Texture* square, *text;

	int time, totoalTime, volume;
	int win_h, win_w;

	int x = 0, y = 0, w = 0, h = 0;
	uint8_t alpha;

	void init();
	void setTime(int t) { time = t; };
	void setTotalTime(int t) { totoalTime = t; };
	void drawBall();
	void drawText(const string &text);
	void drawUI(uint8_t alpha, int time, int totoalTime, int volume);
	string convertTimeToString(int time);
};


#include "BigPotUI.h"
#include "math.h"


BigPotUI::BigPotUI()
{
	square = control->createSquareTexture();
}


BigPotUI::~BigPotUI()
{
}

void BigPotUI::drawBall()
{	
	if (alpha == 0)
		return;
	control->setTextureAlphaMod(square, alpha);

	int d = 10, x, y;
	y = win_h - 15;
	control->renderCopy(square, -100, y + d / 2 - 1, win_w + 200, 2);
	control->renderCopy(square,  
		(int)(1.0 * time / totoalTime * win_w - 5), y, d, d);

	x = win_w - 20;
	y = 40;
	control->renderCopy(square, x, 40, 2, BP_AUDIO_MIX_MAXVOLUME / 2 + d);
	control->renderCopy(square, x - d / 2 + 1, y - volume / 2 + BP_AUDIO_MIX_MAXVOLUME / 2, d, d);
}

void BigPotUI::drawText(const string &text)
{
	control->drawText("c:/windows/fonts/cambriai.ttf", text, 20, win_w - 10, 10, alpha, BP_ALIGN_RIGHT);
}

void BigPotUI::drawUI(uint8_t alpha, int time, int totoalTime, int volume)
{
	this->alpha = alpha;
	if (alpha == 0)
		return;
	control->getWindowSize(win_w, win_h);
	this->time = time;
	this->totoalTime = totoalTime;
	this->volume = volume;
	drawBall();	
	drawText(convertTimeToString(time) + " / " + convertTimeToString(totoalTime));
}

std::string BigPotUI::convertTimeToString(int time)
{
	char s[256];
	sprintf(s, "%d:%02d:%04.2f",
		time / 3600000, time % 3600000 / 60000, time % 60000 / 1e3);
	return s;
}

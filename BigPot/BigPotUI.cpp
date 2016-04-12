#include "BigPotUI.h"
#include "math.h"


BigPotUI::BigPotUI()
{

}


BigPotUI::~BigPotUI()
{
	engine_->destroyTexture(_square);
	//engine->destroyTexture(text);
}

void BigPotUI::drawBall()
{	
	if (_alpha == 0)
		return;
	engine_->setTextureAlphaMod(_square, _alpha);

	int d = 10, x, y;
	y = _win_h - 15;
	engine_->renderCopy(_square, -100, y + d / 2 - 1, _win_w + 200, 2);
	engine_->renderCopy(_square,  
		(int)(1.0 * _time / _totoalTime * _win_w - 5), y, d, d);

	x = _win_w - 20;
	y = 40;
	engine_->renderCopy(_square, x, 40, 2, BP_AUDIO_MIX_MAXVOLUME / 2 + d);
	engine_->renderCopy(_square, x - d / 2 + 1, y - _volume / 2 + BP_AUDIO_MIX_MAXVOLUME / 2, d, d);
}

void BigPotUI::drawText(const string &text)
{
#ifdef _WIN32
    const char* s = "c:/windows/fonts/cambria.ttc";
	if (!fileExist(s))
		s = "c:/windows/fonts/cambria.ttf";
#else
    const char* s = "/System/Library/Fonts/Palatino.ttc";
#endif
    engine_->drawText(s, text, 20, _win_w - 10, 10, _alpha, BP_ALIGN_RIGHT);
}

void BigPotUI::drawUI(uint8_t alpha, int time, int totoalTime, int volume)
{
	this->_alpha = alpha;
	if (alpha == 0)
		return;
	//_win_w = engine_->getWindowsWidth();
	//_win_h = engine_->getWindowsHeight();
	engine_->getWindowSize(_win_w, _win_h);
	this->_time = time;
	this->_totoalTime = totoalTime;
	this->_volume = volume;
	drawBall();	
	drawText(convertTimeToString(time) + " / " + convertTimeToString(totoalTime));
}

std::string BigPotUI::convertTimeToString(int time)
{
	char s[256];
	sprintf(s, "%d:%02d:%05.2f",
		time / 3600000, time % 3600000 / 60000, time % 60000 / 1e3);
	return s;
}

void BigPotUI::init()
{
	_square = engine_->createSquareTexture(30);
}

void BigPotUI::destory()
{
	engine_->destroyTexture(_square);
}

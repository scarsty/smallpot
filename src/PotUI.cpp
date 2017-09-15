#include "PotUI.h"
#include "math.h"
#include "Config.h"
#include "File.h"

PotUI::PotUI()
{

}


PotUI::~PotUI()
{
    engine_->destroyTexture(_square);
    //engine->destroyTexture(text);
}

void PotUI::drawBall()
{
    if (_alpha == 0)
    { return; }
    engine_->setTextureAlphaMod(_square, _alpha);

    int d = 10, x, y;
    y = _win_h - 15;
    engine_->renderCopy(_square, -100, y + d / 2 - 1, _win_w + 200, 2);
    engine_->renderCopy(_square, 1.0 * _time / _totoalTime * _win_w - 5, y, d, d);

    int one_square = BP_AUDIO_MIX_MAXVOLUME / 8;
    int v = _volume;
    x = _win_w - 40;
    y = 30;
    for (int i = 0; i < 8; i++)
    {
        int h = (i + 1) * 2;
        v -= one_square;
        double r = 1;
        if (v < 0) { r = 1.0 * (one_square + v) / one_square; }
        int hc = r * h;
        engine_->renderCopy(_square, x + i * 3, y - hc, 2, hc);
        if (v < 0) { break; }
    }

    //x = _win_w - 10 - BP_AUDIO_MIX_MAXVOLUME / 2 - d;
    //y = 40;
    //engine_->renderCopy(_square, x - 2, y - 2, 6, 2);
    //engine_->renderCopy(_square, x - 2, y + BP_AUDIO_MIX_MAXVOLUME / 2 + d, 6, 2);
    //engine_->renderCopy(_square, x, y, BP_AUDIO_MIX_MAXVOLUME / 2 + d, 2);
    //engine_->renderCopy(_ball, x + _volume / 2, y - d / 2 + 1, d, d);
}

void PotUI::drawText(const std::string& text)
{
    engine_->drawText(_fontname.c_str(), text, 20, _win_w - 50, 10, _alpha, BP_ALIGN_RIGHT);
    //engine_->drawText(_fontname.c_str(), std::to_string(_volume / 128.0)+"%", 20, _win_w - 10, 35, _alpha, BP_ALIGN_RIGHT);
}

void PotUI::drawUI(uint8_t alpha, int time, int totoalTime, int volume)
{
    this->_alpha = alpha;
    if (alpha == 0)
    { return; }
    //_win_w = engine_->getWindowsWidth();
    //_win_h = engine_->getWindowsHeight();
    engine_->getWindowSize(_win_w, _win_h);
    this->_time = time;
    this->_totoalTime = totoalTime;
    this->_volume = volume;
    drawBall();
    drawText(convertTimeToString(time) + " / " + convertTimeToString(totoalTime));
}

std::string PotUI::convertTimeToString(int time)
{
    char s[256];
    sprintf(s, "%d:%02d:%05.2f", time / 3600000, time % 3600000 / 60000, time % 60000 / 1e3);
    return s;
}

void PotUI::init()
{
    _square = engine_->createSquareTexture(40);
    _ball = engine_->createBallTexture(40);
    _fontname = config_->getString("ui_font");
    if (!File::fileExist(_fontname))
    {
#ifdef _WIN32
        _fontname = "c:/windows/fonts/cambria.ttc";
        if (!File::fileExist(_fontname))
        { _fontname = "c:/windows/fonts/cambria.ttf"; }
#else
        _fontname = "/System/Library/Fonts/Palatino.ttc";
#endif
    }
}

void PotUI::destory()
{
    if (config_->getString("ui_font") == "")
    { config_->setString(_fontname, "ui_font"); }
    engine_->destroyTexture(_square);
}

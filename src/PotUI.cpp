#include "PotUI.h"
#include "math.h"
#include "Config.h"
#include "File.h"

PotUI::PotUI()
{
}


PotUI::~PotUI()
{
}

void PotUI::drawBall()
{
    if (alpha_ == 0)
    {
        return;
    }
    engine_->setTextureAlphaMod(square_, alpha_ / 2);
    engine_->setTextureAlphaMod(ball_, alpha_);
    int d = 10, x, y;
    y = win_h_ - 15;
    engine_->renderCopy(square_, -100, y + d / 2 - 1, win_w_ + 200, 2);
    engine_->renderCopy(ball_, 1.0 * time_ / totoal_time_ * win_w_ - d / 2, y, d, d);

    int one_square = BP_AUDIO_MIX_MAXVOLUME / 8;
    int v = volume_;
    x = win_w_ - 40;
    y = 30;
    for (int i = 0; i < 8; i++)
    {
        int h = (i + 1) * 2;
        v -= one_square;
        double r = 1;
        if (v < 0)
        {
            r = 1.0 * (one_square + v) / one_square;
        }
        int hc = r * h;
        engine_->renderCopy(square_, x + i * 3, y - hc, 2, hc);
        if (v < 0)
        {
            break;
        }
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
    engine_->drawText(fontname_.c_str(), text, 20, win_w_ - 50, 10, alpha_, BP_ALIGN_RIGHT);
    //engine_->drawText(_fontname.c_str(), std::to_string(_volume / 128.0)+"%", 20, _win_w - 10, 35, _alpha, BP_ALIGN_RIGHT);
}

void PotUI::drawUI(uint8_t alpha, int time, int totoalTime, int volume)
{
    this->alpha_ = alpha;
    if (alpha == 0)
    {
        text_ = "";
        return;
    }
    //_win_w = engine_->getWindowsWidth();
    //_win_h = engine_->getWindowsHeight();
    engine_->getWindowSize(win_w_, win_h_);
    this->time_ = time;
    this->totoal_time_ = totoalTime;
    this->volume_ = volume;
    drawBall();
    if (text_ == "")
    {
        drawText(convertTimeToString(time) + " / " + convertTimeToString(totoalTime));
    }
    else
    {
        drawText(text_);
    }
}

std::string PotUI::convertTimeToString(int time)
{
    char s[256];
    sprintf(s, "%d:%02d:%02d", time / 3600000, time % 3600000 / 60000, time % 60000 / 1000);
    return s;
}

void PotUI::init()
{
    square_ = engine_->createSquareTexture(40);
    ball_ = engine_->createBallTexture(50);
    fontname_ = Config::getInstance()->getString("ui_font");
    if (!File::fileExist(fontname_))
    {
#ifdef _WIN32
        fontname_ = "C:\\Windows\\Fonts\\Cambria.ttc";
        if (!File::fileExist(fontname_))
        {
            fontname_ = "C:\\Windows\\Fonts\\Cambria.ttf";
        }
#else
        fontname_ = "/System/Library/Fonts/Palatino.ttc";
#endif
    }
}

void PotUI::destory()
{
    if (Config::getInstance()->getString("ui_font") == "")
    {
        Config::getInstance()->setString("ui_font", fontname_);
    }
    engine_->destroyTexture(square_);
    engine_->destroyTexture(ball_);
}

#include "PotUI.h"
#include "Config.h"
#include "File.h"
#include "math.h"

PotUI::PotUI()
{
}

PotUI::~PotUI()
{
}

void PotUI::drawText(const std::string& text)
{
    engine_->drawText(fontname_.c_str(), text, 24, win_w_ - 10, win_h_ - 50, alpha_, BP_ALIGN_RIGHT);
    //engine_->drawText(_fontname.c_str(), std::to_string(_volume / 128.0)+"%", 20, _win_w - 10, 35, _alpha, BP_ALIGN_RIGHT);
}

void PotUI::drawUI(uint8_t alpha, int time, int totoalTime, int volume, bool pause)
{
    this->alpha_ = alpha;
    if (alpha == 0)
    {
        text_ = "";
        return;
    }

    engine_->getWindowSize(win_w_, win_h_);
    this->time_ = time;
    this->totoal_time_ = totoalTime;

    //engine_->renderCopy(square2_, 0, win_h_ - 75, win_w_, 75);

    if (alpha_ != 0)
    {
        int x, y;
        y = win_h_ - 12;
        engine_->setColor(square_, { 255, 255, 255 }, alpha_ / 2);
        engine_->renderCopy(square_, 0, y - 1, win_w_, 4);
        //int xm, ym;
        //engine_->getMouseState(xm, ym);
        //if (inProcess(xm, ym)>0)
        //{
        //    engine_->renderCopy(square_, 0, y - 1, xm, 4);
        //}
        engine_->setColor(square_, { 255, 0, 0 }, alpha_);
        engine_->renderCopy(square_, 0, y - 1, 1.0 * time_ / totoal_time_ * win_w_, 4);
    }

    engine_->setColor(square_, { 255, 255, 255 }, alpha_);
    setAlpha(alpha_);

    if (text_ == "")
    {
        drawText(convertTimeToString(time) + "/" + convertTimeToString(totoalTime));
    }
    else
    {
        drawText(text_);
    }

    button_y_ = win_h_ - 45;
    //engine_->setTextureAlphaMod(triangle1_, alpha_);
    //engine_->setTextureAlphaMod(triangle2_, alpha_);
    //engine_->renderCopy(triangle2_, button_x, button_y_, button_w_ / 2, button_h_);
    //engine_->renderCopy(triangle2_, button_x + 5, button_y_, button_w_ / 2, button_h_);
    //button_x += 15;

    int mouse_x, mouse_y;
    engine_->getMouseState(mouse_x, mouse_y);

    int in_button = inButton(mouse_x, mouse_y);
    //pause button
    int button_x = button_x_;
    if (in_button == 1)
    {
        setAlpha(alpha_ * 1.5);
    }
    if (pause)
    {
        engine_->renderCopy(triangle1_, button_x, button_y_, button_w_, button_h_);
    }
    else
    {
        engine_->renderCopy(square_, button_x, button_y_, 8, button_h_);
        engine_->renderCopy(square_, button_x + 12, button_y_, 8, button_h_);
    }
    setAlpha(alpha_);

    //next button
    button_x += button_w_ + 10;
    if (in_button == 2)
    {
        setAlpha(alpha_ * 1.5);
    }
    engine_->renderCopy(triangle1_, button_x, button_y_, button_w_ / 2, button_h_);
    engine_->renderCopy(square_, button_x + 14, button_y_, 6, button_h_);
    setAlpha(alpha_);

    int one_square = BP_AUDIO_MIX_MAXVOLUME / 8;
    int v = volume;
    int x = button_x + 30;
    int y = button_y_ + 20;
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
    setAlpha(alpha_);
}

std::string PotUI::convertTimeToString(int time)
{
    char s[256];
    sprintf(s, "%d:%02d:%02d", time / 3600000, time % 3600000 / 60000, time % 60000 / 1000);
    return s;
}

double PotUI::inProcess(int x, int y)
{
    if (y > win_h_ - 24)
    {
        double p = 1.0 * x / win_w_;
        if (p >= 0 && p <= 1)
        {
            return p;
        }
    }
    return -1;
}

int PotUI::inButton(int x, int y)
{
    if (y >= button_y_ && y <= button_y_ + button_h_)
    {
        int button_x = button_x_;
        if (x >= button_x && x <= button_x + button_w_)
        {
            return 1;
        }
        button_x += button_w_ + 10;
        if (x >= button_x && x <= button_x + button_w_)
        {
            return 2;
        }
        //button_x += button_w_ + 10;
        //if (x >= button_x && x <= button_x + button_w_)
        //{
        //    return 2;
        //}
    }
    return -1;
}

void PotUI::init()
{
    square_ = engine_->createSquareTexture(40);
    ball_ = engine_->createSpecialTexture(50);
    triangle1_ = engine_->createSpecialTexture(200, 1);
    triangle2_ = engine_->createSpecialTexture(200, 2);
    square2_ = engine_->createSpecialTexture(75, 3);
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
    engine_->destroyTexture(triangle1_);
    engine_->destroyTexture(triangle2_);
}

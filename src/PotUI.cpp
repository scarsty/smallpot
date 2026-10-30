#include "PotUI.h"
#include "filefunc.h"
#include "math.h"
#include "strfunc.h"

#ifndef _WINDLL
#include "Config.h"
#include "Font.h"
#endif

PotUI::PotUI()
{
}

PotUI::~PotUI()
{
}

void PotUI::init()
{
    square_ = createSquareTexture(40);
    ball_ = createSpecialTexture(50);
    triangle_ = createSpecialTexture(200, 1);
    triangle2_ = createSpecialTexture(200, 2);
    to_full_screen_ = createSpecialTexture(20, 4);
    to_window_ = createSpecialTexture(20, 5);
    hollow_ = createSpecialTexture(20, 6);
    frame_ = createSpecialTexture(20, 7);
    //fontname_ = Config::getInstance()->getString("ui_font");
    if (!filefunc::fileExist(fontname_))
    {
#ifdef _WIN32
        fontname_ = "C:/Windows/Fonts/calibri.ttf";
#else
        fontname_ = "/System/Library/Fonts/Palatino.ttc";
#endif
    }
}

void PotUI::drawText(const std::string& text)
{
#ifndef _WINDLL
    const int text_y = button_y_ + button_h_ - ui_font_size_;
    Font::getInstance()->drawText(fontname_.c_str(), text, ui_font_size_, getButtonPos(ButtonVolume) - 10, text_y, alpha_, ALIGN_RIGHT);
#endif
}

void PotUI::drawUI(int time, int totoal_time, float volume, bool pause)
{
#ifndef _WINDLL
    engine_->getWindowSize(win_w_, win_h_);
    button_y_ = win_h_ - button_bottom_offset_;
    int in_button = inButton();
    double process = inProcess();
    if (text_count_ > 0)
    {
        text_count_--;
    }
    if (text_count_ == 0)
    {
        text_ = "";
    }
    if (in_button >= 0 || process >= 0)
    {
        alpha_count_ = 192;
    }
    else
    {
        if (text_count_ == 0 && alpha_count_ > 0)
        {
            alpha_count_ -= 4;
        }
    }
    if (alpha_count_ == 0)
    {
        text_ = "";
        return;
    }

    alpha_ = (std::min)(uint8_t(128), alpha_count_);
    //进度条
    const int progress_y = win_h_ - (progress_hit_height_ + progress_bar_height_) / 2;
    engine_->setColor(square_, { 255, 255, 255, uint8_t(alpha_ / 2) });
    engine_->renderTexture(square_, 0, progress_y, win_w_, progress_bar_height_);
    engine_->setColor(square_, { 255, 0, 0, alpha_ });
    const int progress_width = totoal_time > 0
        ? (std::clamp)(int(1.0 * (std::max)(time, 0) / totoal_time * win_w_), 0, win_w_)
        : 0;
    if (progress_width > 0)
    {
        engine_->renderTexture(square_, 0, progress_y, progress_width, progress_bar_height_);
    }

    //按钮
    engine_->setColor(square_, { 255, 255, 255, alpha_ });
    engine_->setColor(triangle_, { 255, 255, 255, alpha_ });
    engine_->setColor(triangle2_, { 255, 255, 255, alpha_ });
    engine_->setColor(to_full_screen_, { 255, 255, 255, alpha_ });
    engine_->setColor(to_window_, { 255, 255, 255, alpha_ });
    engine_->setColor(hollow_, { 255, 255, 255, alpha_ });

    for (int i = ButtonNone + 1; i < ButtonNone2; i++)
    {
        int button_x = getButtonPos(i);
        int button_y = button_y_;
        switch (i)
        {
        case ButtonPause:
            if (pause)
            {
                engine_->renderTexture(triangle_, button_x, button_y, button_w_, button_h_);
            }
            else
            {
                engine_->renderTexture(square_, button_x, button_y, button_w_ * 0.4, button_h_);
                engine_->renderTexture(square_, button_x + button_w_ * 0.6, button_y, button_w_ * 0.4, button_h_);
            }
            break;
        case ButtonNext:
            engine_->renderTexture(triangle_, button_x, button_y, button_w_ / 2, button_h_);
            engine_->renderTexture(square_, button_x + button_w_ * 0.6, button_y, button_w_ * 0.4, button_h_);
            break;
        case ButtonFullScreen:
            if (engine_->isFullScreen())
            {
                engine_->renderTexture(to_window_, button_x, button_y, button_w_, button_h_);
            }
            else
            {
                engine_->renderTexture(to_full_screen_, button_x, button_y, button_w_, button_h_);
            }
            break;
        case ButtonLeft:
            engine_->renderTexture(triangle2_, button_x, button_y, button_w_ / 2, button_h_);
            engine_->renderTexture(triangle2_, button_x + button_w_ / 2, button_y, button_w_ / 2, button_h_);
            break;
        case ButtonRight:
            engine_->renderTexture(triangle_, button_x, button_y, button_w_ / 2, button_h_);
            engine_->renderTexture(triangle_, button_x + button_w_ / 2, button_y, button_w_ / 2, button_h_);
            break;
        case ButtonAudio:
            engine_->renderTexture(square_, button_x, button_y + button_h_ / 4, button_w_ / 2, button_h_ / 2 + button_h_ / 20);
            engine_->renderTexture(triangle2_, button_x + button_w_ / 2, button_y, button_w_ / 2, button_h_);
            break;
        case ButtonSubtitle:
            engine_->renderTexture(hollow_, button_x, button_y, button_w_, button_h_);
            engine_->renderTexture(square_, button_x + button_w_ * 0.2, button_y + button_h_ * 0.6, button_w_ * 0.6, button_h_ * 0.2);
            break;
        }
    }
    if (in_button > 0)
    {
        int x = getButtonPos(in_button) - 2;
        int w = getButtonWidth(in_button) + 4;
        engine_->renderTexture(frame_, x, button_y_ + 2, w, button_h_ + 4);
    }

    int button_x = getButtonPos(ButtonVolume) + 2;
    const int volume_bottom = button_y_ + button_h_;
    int one_square = 8;
    int v = (std::clamp)(int(volume * 128), 0, 128);
    for (int i_v = 0; i_v < 16; i_v++)
    {
        int h = (i_v + 1) * button_h_ / 20;
        v -= one_square;
        double r = 1;
        if (v < 0)
        {
            r = 1.0 * (one_square + v) / one_square;
        }
        const int hc = (std::clamp)(int(r * h), 0, h);
        if (hc > 0)
        {
            engine_->renderTexture(square_, button_x + i_v * 3 * button_w_ / 20, volume_bottom - hc, button_w_ / 10, hc);
        }
        if (v < 0)
        {
            break;
        }
    }

    drawText(convertTimeToString(time) + "/" + convertTimeToString(totoal_time));
    std::string text;
    switch (in_button)
    {
    case ButtonPause:
        if (pause)
        {
            text = "Play";
        }
        else
        {
            text = "Pause";
        }
        break;
    case ButtonNext:
        text = "Next";
        break;
    case ButtonFullScreen:
        if (engine_->isFullScreen())
        {
            text = "Window";
        }
        else
        {
            text = "Full Screen";
        }
        break;
    case ButtonLeft:
        text = "Backward some seconds";
        break;
    case ButtonRight:
        text = "Forward some seconds";
        break;
    case ButtonAudio:
        text = "Switch audio stream";
        break;
    case ButtonSubtitle:
        text = "Switch subtitles";
        break;
        //case ButtonVolume:
        //    text = strfunc::formatString("Volume %5.1f", 100.0 * volume / BP_AUDIO_MIX_MAXVOLUME);
        //    break;
    }
    const int text_y = button_y_ - ui_font_size_ - hover_text_gap_;
    Font::getInstance()->drawText(fontname_.c_str(), text, ui_font_size_, button_x_ - 2, text_y, alpha_, ALIGN_LEFT);
#endif
}

void PotUI::destory()
{
#ifndef _WINDLL
    if (Config::getInstance()["ui_font"].toString() == "")
    {
        Config::getInstance()["ui_font"] = fontname_;
    }
#endif
}

std::string PotUI::convertTimeToString(int time)
{
    return std::format("{}:{:02}:{:02}", time / 3600000, time % 3600000 / 60000, time % 60000 / 1000);
}

void PotUI::setText(std::string t)
{
    //一段时间后文字自动变回时间
    text_count_ = 32;
    alpha_count_ = 192;
    text_ = t;
}

double PotUI::inProcess()
{
    int x, y;
    engine_->getMouseState(x, y);
    engine_->getWindowSize(win_w_, win_h_);
    if (win_w_ > 0 && y >= win_h_ - progress_hit_height_ && y < win_h_)
    {
        double p = 1.0 * x / win_w_;
        if (p >= 0 && p <= 1)
        {
            return p;
        }
    }
    return -1;
}

int PotUI::inButton()
{
    int x, y;
    engine_->getMouseState(x, y);
    engine_->getWindowSize(win_w_, win_h_);
    button_y_ = win_h_ - button_bottom_offset_;
    if (y >= button_y_ - button_hit_padding_y_
        && y < button_y_ + button_h_ + button_hit_padding_y_)
    {
        for (int i = ButtonNone + 1; i < ButtonNone2; i++)
        {
            if (x >= getButtonPos(i) - button_hit_padding_x_
                && x < getButtonPos(i) + getButtonWidth(i) + button_hit_padding_x_)
            {
                return i;
            }
        }
        return 0;
    }
    return -1;
}

int PotUI::getButtonWidth(int b)
{
    if (b == ButtonVolume)
    {
        return 49 * button_w_ / 20;
    }
    return button_w_;
}

int PotUI::getButtonPos(int b)
{
    if (b == ButtonVolume)
    {
        return win_w_ - 20 - getButtonWidth(ButtonVolume);
    }
    return button_x_ + (b - 1) * (button_w_ + button_spacing_);
}

Texture* PotUI::createSquareTexture(int size)
{
    int d = size;
    auto square_s = SDL_CreateSurface(d, d, SDL_PIXELFORMAT_RGBA8888);
    SDL_FillSurfaceRect(square_s, nullptr, 0xffffffff);
    auto square = SDL_CreateTextureFromSurface(engine_->getRenderer(), square_s);
    SDL_SetTextureBlendMode(square, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(square, 128);
    SDL_DestroySurface(square_s);
    return square;
}

Texture* PotUI::createSpecialTexture(int size, int mode)
{
    int d = size;
    auto ball_s = SDL_CreateSurface(d, d, SDL_PIXELFORMAT_RGBA8888);
    SDL_FillSurfaceRect(ball_s, nullptr, 0);
    SDL_Rect r = { 0, 0, 1, 1 };
    auto& x = r.x;
    auto& y = r.y;
    double c = (d - 1) / 2.0;
    for (x = 0; x < d; x++)
    {
        for (y = 0; y < d; y++)
        {
            if (mode == 0)
            {
                double ra = sqrt((x - c) * (x - c) + (y - c) * (y - c)) / c;
                double a0 = 0;
                if (ra > 1.05)
                {
                    a0 = 255;
                    //a0 = (ra - 1) * 255 * 4;
                }
                if (ra < 1)
                {
                    a0 = 0;    // (1 - ra) * 255 * 2;
                }
                uint8_t a = a0 > 255 ? 255 : a0;
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 255, 255, 255, a));
            }
            if (mode == 1)
            {
                uint8_t a = 255;
                if (1.0 * abs(y - size / 2) / (size - x) > 0.5)
                {
                    a = 0;
                }
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 255, 255, 255, a));
            }
            if (mode == 2)
            {
                uint8_t a = 255;
                if (1.0 * abs(y - size / 2) / x > 0.5)
                {
                    a = 0;
                }
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 255, 255, 255, a));
            }
            if (mode == 3)
            {
                uint8_t a = 255;
                a = 225.0 / d * y;
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 0, 0, 0, a));
            }
            if (mode == 4)
            {
                uint8_t a = 255;
                double center = (d - 1) / 2.0;
                if ((abs(x - center) < d * 0.35 && abs(y - center) < d * 0.35)
                    || (abs(x - center) < d * 0.1 || abs(y - center) < d * 0.1))
                {
                    a = 0;
                }
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 255, 255, 255, a));
            }
            if (mode == 5)
            {
                uint8_t a = 255;
                double center = (d - 1) / 2.0;
                if ((abs(x - center) > d * 0.25 && abs(y - center) > d * 0.25)
                    || (abs(x - center) < d * 0.1 || abs(y - center) < d * 0.1))
                {
                    a = 0;
                }
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 255, 255, 255, a));
            }
            if (mode == 6)
            {
                uint8_t a = 255;
                double center = (d - 1) / 2.0;
                if ((abs(x - center) < d * 0.35 && abs(y - center) < d * 0.35))
                {
                    a = 0;
                }
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 255, 255, 255, a));
            }
            if (mode == 7)
            {
                uint8_t a = 0;
                if (y == size - 1)
                {
                    a = 255;
                }
                SDL_FillSurfaceRect(ball_s, &r, SDL_MapSurfaceRGBA(ball_s, 255, 255, 255, a));
            }
        }
    }
    auto ball = SDL_CreateTextureFromSurface(engine_->getRenderer(), ball_s);
    SDL_SetTextureBlendMode(ball, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(ball, 128);
    SDL_DestroySurface(ball_s);
    return ball;
}

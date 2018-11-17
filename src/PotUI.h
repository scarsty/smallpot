#pragma once
#include "PotBase.h"

class PotUI : public PotBase
{
public:
    PotUI();
    virtual ~PotUI();

private:
    BP_Texture *square_ = nullptr, *square2_ = nullptr, *ball_ = nullptr, *triangle1_ = nullptr, *triangle2_ = nullptr;

    int time_, totoal_time_, volume_;
    int win_w_, win_h_;

    //int x = 0, y = 0, w = 0, h = 0;
    uint8_t alpha_;
    std::string fontname_;
    std::string text_;

    int button_x_ = 10, button_y_, button_w_ = 20, button_h_ = 20;

    int left_ = 60, right_ = 40;
    int d_ = 20;

public:
    void init();
    void setTime(int t) { time_ = t; }
    void setTotalTime(int t) { totoal_time_ = t; }
    void drawText(const std::string& text);
    void drawUI(uint8_t alpha, int time, int totoalTime, int volume, bool pause);
    void destory();
    std::string convertTimeToString(int time);
    void setText(std::string t) { text_ = t; }
    double inProcess(int x, int y);
    int inButton(int x, int y);
};

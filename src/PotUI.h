#pragma once
#include "PotBase.h"

class PotUI : public PotBase
{
public:
    PotUI();
    virtual ~PotUI();
private:
    BP_Texture* square_, *text_, *ball_;

    int time_, totoal_time_, volume_;
    int win_w_, win_h_;

    //int x = 0, y = 0, w = 0, h = 0;
    uint8_t alpha_;
    std::string fontname_;
public:
    void init();
    void setTime(int t) { time_ = t; }
    void setTotalTime(int t) { totoal_time_ = t; }
    void drawBall();
    void drawText(const std::string& text);
    void drawUI(uint8_t alpha, int time, int totoalTime, int volume);
    void destory();
    std::string convertTimeToString(int time);
};


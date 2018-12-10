#pragma once
#include "PotBase.h"

class PotUI : public PotBase
{
public:
    PotUI();
    virtual ~PotUI();

private:
    BP_Texture *square_ = nullptr, *square2_ = nullptr, *ball_ = nullptr, *triangle1_ = nullptr, *triangle2_ = nullptr;

    int win_w_, win_h_;

    //int x = 0, y = 0, w = 0, h = 0;
    uint8_t alpha_, alpha_count_;
    std::string fontname_;
    std::string text_;
    int text_count_ = 32;

    int button_x_ = 10, button_y_, button_w_ = 20, button_h_ = 20;

    int left_ = 60, right_ = 40;
    int d_ = 20;

public:
    void init();
    void drawText(const std::string& text);
    void drawUI(int time, int totoal_time, int volume, bool pause);
    void destory();
    std::string convertTimeToString(int time);
    void setText(std::string t);
    double inProcess();
    int inButton();
};

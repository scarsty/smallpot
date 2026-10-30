#pragma once
#include "PotBase.h"

class PotUI : public PotBase
{
public:
    PotUI();
    virtual ~PotUI();

    enum
    {
        ButtonNone = 0,
        ButtonPause,
        ButtonNext,
        ButtonFullScreen,
        ButtonLeft,
        ButtonRight,
        ButtonAudio,
        ButtonSubtitle,
        ButtonVolume,
        ButtonNone2,
    };

private:
    Texture *square_ = nullptr, *ball_ = nullptr, *triangle_ = nullptr, *triangle2_ = nullptr, *hollow_ = nullptr;
    Texture *to_full_screen_ = nullptr, *to_window_ = nullptr;
    Texture *frame_ = nullptr;
    int win_w_, win_h_;

    //int x = 0, y = 0, w = 0, h = 0;
    uint8_t alpha_ = 128, alpha_count_ = 192;
    std::string fontname_;
    std::string text_;
    int text_count_ = 32;

    static constexpr int progress_hit_height_ = 40;
    static constexpr int progress_bar_height_ = 8;
    static constexpr int button_bottom_offset_ = 92;
    static constexpr int button_spacing_ = 14;
    static constexpr int ui_font_size_ = 32;
    static constexpr int hover_text_gap_ = 6;
    static constexpr int button_hit_padding_x_ = 5;
    static constexpr int button_hit_padding_y_ = 10;
    int button_x_ = 10, button_y_ = 0, button_w_ = 40, button_h_ = 40;

public:
    void init();
    void drawText(const std::string& text);
    void drawUI(int time, int totoal_time, float volume, bool pause);
    void destory();
    std::string convertTimeToString(int time);
    void setText(std::string t);
    double inProcess();
    int inButton();
    int getButtonWidth(int b);
    int getButtonPos(int b);

private:
    Texture* createSquareTexture(int size);
    Texture* createSpecialTexture(int size, int mode = 0);
};

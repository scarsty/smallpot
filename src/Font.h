#pragma once
#include <string>
#include <map>
#include "Engine.h"

class Font
{
private:
    Font() {}
    ~Font();
    static Font font_;
    std::map<std::string, BP_Texture*> buffer_;  //缓存所有已经画过的字体

    BP_Texture* indexTex(const std::string& fontname, int size, uint16_t c);
public:
    static Font* getInstance() { return &font_; };
    int getTextWidth(const std::string& fontname, const std::string& text, int size);
    void draw(const std::string& fontname, const std::string& text, int size, int x, int y, BP_Color color = { 255, 255, 255, 255 }, uint8_t alpha = 255);
};


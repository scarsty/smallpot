#pragma once
#include "Engine.h"
#include <map>
#include <string>

class Font
{
private:
    Font();
    ~Font();
    std::map<std::string, BP_Texture*> buffer_;    //缓存所有已经画过的字体
    BP_Texture* indexTex(const std::string& fontname, uint16_t c, int size);

public:
    static Font* getInstance()
    {
        static Font f;
        return &f;
    }
    int getTextWidth(const std::string& fontname, const std::string& text, int size);
    void draw(const std::string& fontname, const std::string& text, int size, int x, int y, BP_Color color = { 255, 255, 255, 255 }, uint8_t alpha = 255);
    void clearBuffer();
    int getBufferSize() { return buffer_.size(); }

    BP_Texture* createTextTexture2(const std::string& fontname, const std::string& s, int size);
    void drawText(const std::string& fontname, const std::string& text, int size, int x, int y, uint8_t alpha, int align);
    void drawSubtitle(const std::string& fontname, const std::string& text, int size, int x, int y, uint8_t alpha, int align);
};

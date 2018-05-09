#include "Font.h"
#include "PotConv.h"

Font Font::font_;

Font::~Font()
{
    for (auto buffer : buffer_)
    {
        Engine::destroyTexture(buffer.second);
    }
}

BP_Texture* Font::indexTex(const std::string& fontname, int size, uint16_t c)
{
    auto index = fontname + "-" + std::to_string(size * 0x1000000 + c);
    if (buffer_.count(index) == 0)
    {
        uint16_t c2[2] = { 0 };
        c2[0] = c;
        auto s = PotConv::cp936toutf8((char*)(c2));
        BP_Texture* tex = Engine::getInstance()->createTextTexture(fontname, s, size, { 255, 255, 255, 255 });
        buffer_[index] = tex;
    }
    return buffer_[index];
}

int Font::getTextWidth(const std::string& fontname, const std::string& text, int size)
{
    int p = 0;    
    int x = 0;
    while (p < text.size())
    {
        int w = size;
        uint16_t c = (uint8_t)text[p];
        p++;
        if (c > 128)
        {
            c += (uint8_t)text[p] * 256;
            p++;
        }
        auto tex = indexTex(fontname, size, c);
        Engine::getInstance()->queryTexture(tex, &w, nullptr);
        x += w;
    }
    return x;
}

void Font::draw(const std::string& fontname, const std::string& text, int size, int x, int y, BP_Color color, uint8_t alpha)
{
    int p = 0;
    while (p < text.size())
    {
        int w = size, h = size;
        uint16_t c = (uint8_t)text[p];
        p++;
        if (c > 128)
        {
            c += (uint8_t)text[p] * 256;
            p++;
        }
        auto tex = indexTex(fontname, size, c);
        Engine::getInstance()->queryTexture(tex, &w, &h);
        //Engine::getInstance()->setColor(tex, { uint8_t(color.r / 2), uint8_t(color.g / 2), uint8_t(color.b / 2), color.a }, alpha);
        //Engine::getInstance()->renderCopy(tex, x + 1, y, w, h);
        Engine::getInstance()->setColor(tex, color, alpha);
        Engine::getInstance()->renderCopy(tex, x, y, w, h);

        x += w;
    }
}




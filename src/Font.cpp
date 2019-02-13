#include "Font.h"
#include "PotConv.h"

 Font::Font()
{
}

Font::~Font()
{
}

BP_Texture* Font::indexTex(const std::string& fontname, uint16_t c, int size)
{
    auto index = fontname + "-" + std::to_string(c) + "-" + std::to_string(size);
    if (buffer_.count(index) == 0)
    {
        uint16_t c2[2] = { 0 };
        c2[0] = c;
        auto s = PotConv::cp936toutf8((char*)(c2));
        buffer_[index] = Engine::getInstance()->createTextTexture(fontname, s, size, { 255, 255, 255, 255 });
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
        auto tex = indexTex(fontname, c, size);
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
        auto tex = indexTex(fontname, c, size);
        Engine::getInstance()->queryTexture(tex, &w, &h);
        //Engine::getInstance()->setColor(tex, { uint8_t(color.r / 2), uint8_t(color.g / 2), uint8_t(color.b / 2), color.a }, alpha);
        //Engine::getInstance()->renderCopy(tex, x + 1, y, w, h);
        Engine::getInstance()->setColor(tex, color, alpha);
        Engine::getInstance()->renderCopy(tex, x, y, w, h);
        x += w;
    }
}

void Font::clearBuffer()
{
    for (auto& f : buffer_)
    {
        Engine::destroyTexture(f.second);
    }
    buffer_.clear();
}

void Font::drawText(const std::string& fontname, const std::string& text, int size, int x, int y, uint8_t alpha, int align)
{
    if (alpha <= 0 || size <= 0)
    {
        return;
    }

    int w = Font::getInstance()->getTextWidth(fontname, text, size);
    switch (align)
    {
    case BP_ALIGN_LEFT:
        break;
    case BP_ALIGN_RIGHT:
        x = x - w;
        break;
    case BP_ALIGN_MIDDLE:
        x = x - w / 2;
        break;
    }
    Font::getInstance()->draw(fontname, text, size, x, y, { 255, 255, 255, 255 }, alpha);
}

void Font::drawSubtitle(const std::string& fontname, const std::string& text, int size, int x, int y, uint8_t alpha, int align)
{
    if (alpha == 0)
    {
        return;
    }
    drawText(fontname, text, size, x, y, alpha, align);
}
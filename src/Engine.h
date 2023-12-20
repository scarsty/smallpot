#pragma once

extern "C"
{
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
}

#include <algorithm>
#include <atomic>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "fmt1.h"

//这里是底层部分，将SDL的函数均封装了一次
//如需更换底层，则要重新实现下面的全部功能，并重新定义全部常数和类型
#define BP_AUDIO_MIX_MAXVOLUME SDL_MIX_MAXVOLUME

using AudioCallback = std::function<void(uint8_t*, int)>;
using BP_Renderer = SDL_Renderer;
using BP_Window = SDL_Window;
using BP_Texture = SDL_Texture;
using BP_Rect = SDL_Rect;
using BP_Color = SDL_Color;

enum BP_Align
{
    BP_ALIGN_LEFT,
    BP_ALIGN_MIDDLE,
    BP_ALIGN_RIGHT
};

#define BP_WINDOWPOS_CENTERED SDL_WINDOWPOS_CENTERED

#define RMASK (0xff0000)
#define GMASK (0xff00)
#define BMASK (0xff)
#define AMASK (0xff000000)

//声音类型在其他文件中未使用
using BP_AudioSpec = SDL_AudioSpec;
//这里直接使用SDL的事件结构，如果更换底层需重新实现一套相同的
using BP_Event = SDL_Event;

class Engine
{
private:
    Engine();
    virtual ~Engine();

public:
    static Engine* getInstance()
    {
        static Engine e;
        return &e;
    }
    //图形相关
private:
    bool inited_ = false;
    BP_Window* window_ = nullptr;
    BP_Renderer* renderer_ = nullptr;
    bool renderer_self_ = false;
    int window_mode_ = 0;    //0-窗口和渲染器自行创建，1-窗口和渲染器由外部创建
    BP_Texture* tex_ = nullptr, * tex2_ = nullptr, * logo_ = nullptr;
    BP_AudioSpec audio_spec_;
    BP_Rect rect_;
    bool full_screen_ = false;
    bool keep_ratio_ = true;

    int start_w_ = 320, start_h_ = 150;    //320, 150
    int win_w_, win_h_, min_x_, min_y_, max_x_, max_y_;
    double rotation_ = 0;
    int ratio_x_ = 1, ratio_y_ = 1;

public:
    int init(void* handle = nullptr, int handle_type = 0, int maximized = 0);

    void getWindowSize(int& w, int& h) { SDL_GetWindowSize(window_, &w, &h); }
    void getWindowMaxSize(int& w, int& h) { SDL_GetWindowMaximumSize(window_, &w, &h); }
    int getWindowWidth();
    int getWindowHeight();
    int getMaxWindowWidth() { return max_x_ - min_x_; }
    int getMaxWindowHeight() { return max_y_ - min_y_; }
    void getWindowPosition(int& x, int& y) { SDL_GetWindowPosition(window_, &x, &y); }
    bool getWindowIsMaximized();
    void setWindowIsMaximized(bool b);
    void setWindowSize(int w, int h);
    void setStartWindowSize(int w, int h)
    {
        start_w_ = w;
        start_h_ = h;
    }
    void setWindowPosition(int x, int y);
    void setWindowTitle(const std::string& str) { SDL_SetWindowTitle(window_, str.c_str()); }
    void raiseWindow() { SDL_RaiseWindow(window_); }
    BP_Renderer* getRenderer() { return renderer_; }

    void createMainTexture(int pix_fmt, int w, int h);
    void resizeMainTexture(int w, int h);

    void setPresentPosition();    //设置贴图的位置

    void getPresentSize(int& w, int& h)
    {
        w = rect_.w;
        h = rect_.h;
    }
    int getPresentWidth() { return rect_.w; }
    int getPresentHeight() { return rect_.h; }

    void destroyMainTexture() { destroyTexture(tex_); }

    static void destroyTexture(BP_Texture* t);

    BP_Texture* tryMainTexture(BP_Texture* tex) { return tex ? tex : tex_; }
    BP_Texture* createTexture(int pix_fmt, int w, int h);
    void updateYUVTexture(BP_Texture* t, uint8_t* data0, int size0, uint8_t* data1, int size1, uint8_t* data2, int size2);
    void updateARGBTexture(BP_Texture* t, uint8_t* buffer, int pitch);
    int lockTexture(BP_Texture* t, BP_Rect* r, void** pixel, int* pitch);
    void unlockTexture(BP_Texture* t) { SDL_UnlockTexture(tryMainTexture(t)); }

    void renderCopy(BP_Texture* t = nullptr);
    void showLogo() { SDL_RenderCopy(renderer_, logo_, nullptr, nullptr); }
    void renderPresent()
    {
        SDL_RenderPresent(renderer_);
        renderClear();
    };
    void renderClear() { SDL_RenderClear(renderer_); }
    void setTextureAlphaMod(BP_Texture* t, uint8_t alpha) { SDL_SetTextureAlphaMod(t, alpha); };
    void queryTexture(BP_Texture* t, int* w, int* h) { SDL_QueryTexture(t, nullptr, nullptr, w, h); }
    void setRenderTarget(BP_Texture* t) { SDL_SetRenderTarget(renderer_, t); }
    void resetRenderTarget() { SDL_SetRenderTarget(renderer_, nullptr); }
    void createWindow() {}
    void createRenderer() {}
    void renderCopy(BP_Texture* t, int x, int y, int w = 0, int h = 0, int inPresent = 0);
    void destroy();
    bool isFullScreen();
    void toggleFullscreen();
    BP_Texture* loadImage(const std::string& filename);
    bool setKeepRatio(bool b);
    BP_Texture* transBitmapToTexture(const uint8_t* src, uint32_t color, int w, int h, int stride);
    double setRotation(double r) { return rotation_ = r; }
    void resetWindowPosition();
    void setRatio(int x, int y)
    {
        ratio_x_ = x;
        ratio_y_ = y;
    }
    void setColor(BP_Texture* tex, BP_Color c, uint8_t alpha);
    //声音相关
private:
    SDL_AudioDeviceID audio_device_;
    AudioCallback audio_callback_ = nullptr;
    SDL_AudioFormat audio_format_ = AUDIO_S16;

public:
    void pauseAudio(int pause) { SDL_PauseAudioDevice(audio_device_, pause); }
    void closeAudio() { SDL_CloseAudioDevice(audio_device_); };
    static int getMaxVolume() { return BP_AUDIO_MIX_MAXVOLUME; };
    void mixAudio(Uint8* dst, const Uint8* src, Uint32 len, int volume);
    SDL_AudioFormat getAudioFormat() { return audio_format_; }
    int openAudio(int& freq, int& channels, int& size, int minsize, AudioCallback f);
    static void mixAudioCallback(void* userdata, Uint8* stream, int len);
    void setAudioCallback(AudioCallback cb = nullptr) { audio_callback_ = cb; }
    //事件相关
private:
    int time_;

public:
    void delay(const int t) { SDL_Delay(t); }
    uint32_t getTicks() { return SDL_GetTicks(); }
    uint32_t tic() { return time_ = SDL_GetTicks(); }
    void toc()
    {
        if (SDL_GetTicks() != time_) { fmt1::print("{}\n", SDL_GetTicks() - time_); }
    }
    void getMouseState(int& x, int& y) { SDL_GetMouseState(&x, &y); }
    int pollEvent(BP_Event& e) { return SDL_PollEvent(&e); }
    int pushEvent(BP_Event& e) { return SDL_PushEvent(&e); }
    void free(void* mem) { SDL_free(mem); }
    //UI相关
private:
    BP_Texture* square_;

public:
    BP_Texture* createSquareTexture(int size);
    BP_Texture* createSpecialTexture(int size, int mode = 0);
    BP_Texture* createTextTexture(const std::string& fontname, const std::string& text, int size, BP_Color c);
    int showMessage(const std::string& content);
};

//这里直接照搬SDL
//更换底层需自己定义一套
//好像是瞎折腾
enum BP_EventType
{
    BP_FIRSTEVENT = SDL_FIRSTEVENT,
    //按关闭按钮
    BP_QUIT = SDL_QUIT,
    //window
    BP_WINDOWEVENT = SDL_WINDOWEVENT,
    BP_SYSWMEVENT = SDL_SYSWMEVENT,
    //键盘
    BP_KEYDOWN = SDL_KEYDOWN,
    BP_KEYUP = SDL_KEYUP,
    BP_TEXTEDITING = SDL_TEXTEDITING,
    BP_TEXTINPUT = SDL_TEXTINPUT,
    //鼠标
    BP_MOUSEMOTION = SDL_MOUSEMOTION,
    BP_MOUSEBUTTONDOWN = SDL_MOUSEBUTTONDOWN,
    BP_MOUSEBUTTONUP = SDL_MOUSEBUTTONUP,
    BP_MOUSEWHEEL = SDL_MOUSEWHEEL,
    //剪贴板
    BP_CLIPBOARDUPDATE = SDL_CLIPBOARDUPDATE,
    //拖放文件
    BP_DROPFILE = SDL_DROPFILE,
    //渲染改变
    BP_RENDER_TARGETS_RESET = SDL_RENDER_TARGETS_RESET,

    BP_LASTEVENT = SDL_LASTEVENT,
};

enum BP_WindowEventID
{
    BP_WINDOWEVENT_NONE = SDL_WINDOWEVENT_NONE, /**< Never used */
    BP_WINDOWEVENT_SHOWN = SDL_WINDOWEVENT_SHOWN,
    BP_WINDOWEVENT_HIDDEN = SDL_WINDOWEVENT_HIDDEN,
    BP_WINDOWEVENT_EXPOSED = SDL_WINDOWEVENT_EXPOSED,

    BP_WINDOWEVENT_MOVED = SDL_WINDOWEVENT_MOVED,

    BP_WINDOWEVENT_RESIZED = SDL_WINDOWEVENT_RESIZED,
    BP_WINDOWEVENT_SIZE_CHANGED = SDL_WINDOWEVENT_SIZE_CHANGED,
    BP_WINDOWEVENT_MINIMIZED = SDL_WINDOWEVENT_MINIMIZED,
    BP_WINDOWEVENT_MAXIMIZED = SDL_WINDOWEVENT_MAXIMIZED,
    BP_WINDOWEVENT_RESTORED = SDL_WINDOWEVENT_RESTORED,

    BP_WINDOWEVENT_ENTER = SDL_WINDOWEVENT_ENTER,
    BP_WINDOWEVENT_LEAVE = SDL_WINDOWEVENT_LEAVE,
    BP_WINDOWEVENT_FOCUS_GAINED = SDL_WINDOWEVENT_FOCUS_GAINED,
    BP_WINDOWEVENT_FOCUS_LOST = SDL_WINDOWEVENT_FOCUS_LOST,
    BP_WINDOWEVENT_CLOSE = SDL_WINDOWEVENT_CLOSE,
};

enum BP_KeyBoard
{
    BPK_LEFT = SDLK_LEFT,
    BPK_RIGHT = SDLK_RIGHT,
    BPK_UP = SDLK_UP,
    BPK_DOWN = SDLK_DOWN,
    BPK_SPACE = SDLK_SPACE,
    BPK_ESCAPE = SDLK_ESCAPE,
    BPK_RETURN = SDLK_RETURN,
    BPK_DELETE = SDLK_DELETE,
    BPK_BACKSPACE = SDLK_BACKSPACE,
    BPK_0 = SDLK_0,
    BPK_1 = SDLK_1,
    BPK_2 = SDLK_2,
    BPK_3 = SDLK_3,
    BPK_4 = SDLK_4,
    BPK_PLUS = SDLK_PLUS,
    BPK_COMMA = SDLK_COMMA,
    BPK_MINUS = SDLK_MINUS,
    BPK_PERIOD = SDLK_PERIOD,
    BPK_EQUALS = SDLK_EQUALS,
};

enum BP_Button
{
    BP_BUTTON_LEFT = SDL_BUTTON_LEFT,
    BP_BUTTON_MIDDLE = SDL_BUTTON_MIDDLE,
    BP_BUTTON_RIGHT = SDL_BUTTON_RIGHT,
};

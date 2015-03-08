#pragma once

extern "C"
{
#include "sdl/SDL.h"
#include "sdl/SDL_image.h"
#include "sdl/sdl_ttf.h"
}

#include <algorithm>
#include <functional>

using namespace std;

//这里是底层部分，将SDL的函数均封装了一次
//如需更换底层，则要重新实现下面的全部功能，并重新定义全部常数和类型
#define BP_AUDIO_DEVICE_FORMAT AUDIO_S16
#define BP_AUDIO_MIX_MAXVOLUME SDL_MIX_MAXVOLUME

typedef function<void(uint8_t*, int)> AudioCallback;
typedef SDL_Renderer BP_Renderer;
typedef SDL_Window BP_Window;
typedef SDL_Texture BP_Texture;

typedef enum { BP_ALIGN_LEFT, BP_ALIGN_MIDDLE, BP_ALIGN_RIGHT } BP_Align;

#define BP_WINDOWPOS_CENTERED SDL_WINDOWPOS_CENTERED

//声音类型在其他文件中未使用
typedef SDL_AudioSpec BP_AudioSpec;
//这里直接使用SDL的事件结构，如果更换底层需重新实现一套相同的
typedef SDL_Event BP_Event;

class BigPotEngine
{
private:
	BigPotEngine();
	virtual ~BigPotEngine();
private:
	static BigPotEngine _control;
	BigPotEngine* _this;
public:
	static BigPotEngine* getInstance(){ return &_control; };
	//图形相关
private:
	BP_Window* win;
	BP_Renderer* ren;
	BP_Texture* tex;
	BP_AudioSpec want, spec;
	BP_Texture* testTexture(BP_Texture* tex) { return tex ? tex : this->tex; };
public:
	int init();
	void getWindowSize(int &w, int &h) { SDL_GetWindowSize(win, &w, &h); }
	void setWindowSize(int w, int h) { SDL_SetWindowSize(win, w, h); }
	void setWindowPosition(int x, int y)
	{
		SDL_SetWindowPosition(win, x, y);
	}
	void setWindowTitle(const string &str){ SDL_SetWindowTitle(win, str.c_str()); }
	void createMainTexture(int w, int h) { tex = createYUVTexture(w, h); }
	void destroyMainTexture() { destroyTexture(tex); }
	BP_Texture* createYUVTexture(int w, int h) 
	{
		return SDL_CreateTexture(ren, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, w, h);
	};
	void destroyTexture(BP_Texture* t) { SDL_DestroyTexture(t); }
	void updateYUVTexture(BP_Texture* t, uint8_t* data0, int size0, uint8_t* data1, int size1, uint8_t* data2, int size2)
	{
		SDL_UpdateYUVTexture(testTexture(t), nullptr, data0, size0, data1, size1, data2, size2);
	}

	void renderCopy(BP_Texture* t) { SDL_RenderCopy(ren, testTexture(t), nullptr, nullptr); }
	void renderPresent() { SDL_RenderPresent(ren); renderClear(); };
	void renderClear() { SDL_RenderClear(ren); }
	void setTextureAlphaMod(BP_Texture* t, uint8_t alpha) { SDL_SetTextureAlphaMod(t, alpha); };

	void createWindow();
	void createRenderer();
	void renderCopy(BP_Texture* t, int x, int y, int w = 0, int h = 0);
	void destroy()
	{
		SDL_DestroyTexture(tex);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
	}

	//声音相关
private:
	SDL_AudioDeviceID device;
	AudioCallback callback = nullptr;
public:
	void pauseAudio(int pause) { SDL_PauseAudioDevice(device, pause); };
	void closeAudio(){ SDL_CloseAudioDevice(device); };
	int getMaxVolume() { return BP_AUDIO_MIX_MAXVOLUME; };
	void mixAudio(Uint8 * dst, const Uint8 * src, Uint32 len, int volume) 
	{
		SDL_MixAudioFormat(dst, src, BP_AUDIO_DEVICE_FORMAT, len, volume);
	};

	int openAudio(int& freq, int& channels, int& size, int minsize, AudioCallback f);
	static void mixAudioCallback(void* userdata, Uint8* stream, int len);
	void setAudioCallback(AudioCallback cb = nullptr){ callback = cb; };
	//事件相关
private:
	SDL_Event e;
	int _time;
public:
	void delay(const int t) { SDL_Delay(t); }
	uint32_t getTicks(){ return SDL_GetTicks(); }
	uint32_t tic() { return _time = SDL_GetTicks(); }
	void toc() 	{ printf("%d\n", SDL_GetTicks() - _time); }
	void getMouseState(int &x, int& y){ SDL_GetMouseState(&x, &y); };
	int pollEvent(BP_Event& e) { return SDL_PollEvent(&e); };
	void free(void* mem){ SDL_free(mem); }
	//UI相关
private:
	BP_Texture* square;
public:
	BP_Texture* createSquareTexture();
	void drawText(const string &fontname, const string &text, int size, int x, int y, uint8_t alpha, int align);
};

//这里直接照搬SDL
//更换底层需自己定义一套
//好像是瞎折腾
typedef enum
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

	BP_LASTEVENT = SDL_LASTEVENT
} BP_EventType;

typedef enum
{
	BP_WINDOWEVENT_NONE = SDL_WINDOWEVENT_NONE,           /**< Never used */
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
	BP_WINDOWEVENT_CLOSE = SDL_WINDOWEVENT_CLOSE
} BP_WindowEventID;

typedef enum
{
	BPK_LEFT = SDLK_LEFT,
	BPK_RIGHT = SDLK_RIGHT,
	BPK_UP = SDLK_UP,
	BPK_DOWN = SDLK_DOWN,
	BPK_SPACE = SDLK_SPACE,
	BPK_ESCAPE = SDLK_ESCAPE,
	BPK_RETURN = SDLK_RETURN
} BP_KeyBoard;

typedef enum
{
	BP_BUTTON_LEFT= SDL_BUTTON_LEFT,
	BP_BUTTON_MIDDLE= SDL_BUTTON_MIDDLE,
	BP_BUTTON_RIGHT =SDL_BUTTON_RIGHT
} BP_Button;

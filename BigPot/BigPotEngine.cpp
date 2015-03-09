#include "BigPotEngine.h"

BigPotEngine BigPotEngine::_control;

BigPotEngine::BigPotEngine()
{
	_this = &_control;
}


BigPotEngine::~BigPotEngine()
{
	//destroy();
}

void BigPotEngine::renderCopy(BP_Texture* t, int x, int y, int w, int h)
{
	SDL_Rect r = { x, y, w, h };
	SDL_RenderCopy(_ren, t, nullptr, &r);
}

int BigPotEngine::openAudio(int& freq, int& channels, int& size, int minsize, AudioCallback f)
{
	SDL_AudioSpec want;
	SDL_zero(want);
	
	printf("\naudio freq/channels: stream %d/%d, ", freq, channels);
	if (channels <= 2) channels = 2;
	want.freq = freq;
	want.format = BP_AUDIO_DEVICE_FORMAT;
	want.channels = channels;
	want.samples = size;
	want.callback = mixAudioCallback;
	//want.userdata = this;
	want.silence = 0;

	_callback = f;
	//if (useMap())
	{
		want.samples = max(size, minsize);
	}

	_device = 0;
	while (_device == 0)
	{
		_device = SDL_OpenAudioDevice(NULL, 0, &want, &_spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
		want.channels--;
	}
	printf("device %d / %d\n", _spec.freq, _spec.channels);

	if (_device)
	{
		SDL_PauseAudioDevice(_device, 0);
	}
	else
	{
		printf("failed to open audio: %s\n", SDL_GetError());
	}

	freq = _spec.freq;
	channels = _spec.channels;

	return 0;
}

void BigPotEngine::mixAudioCallback(void* userdata, Uint8* stream, int len)
{
	SDL_memset(stream, 0, len);
	if (_control._callback)
	{
		_control._callback(stream, len);
	}
}

BP_Texture* BigPotEngine::createSquareTexture()
{
	int d = 10;
	auto square_s = SDL_CreateRGBSurface(0, d, d, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
	SDL_FillRect(square_s, nullptr, 0xffffffff);
	/*SDL_Rect r = { 0, 0, 1, 1 };
	auto &x = r.x;
	auto &y = r.y;
	for (x = 0; x < d; x++)
	for (y = 0; y < d; y++)
	{
	if ((x - d / 2)*(x - d / 2) + (y - d / 2)*(y - d / 2) < (d / 2) * (d / 2))
	{
	SDL_FillRect(ball_s, &r, 0xffffffff);
	}
	}
	*/
	_square = SDL_CreateTextureFromSurface(_ren, square_s);
	SDL_SetTextureBlendMode(_square, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(_square, 128);
	SDL_FreeSurface(square_s);
	return _square;
}

void BigPotEngine::drawText(const string &fontname, const string &text, int size, int x, int y, uint8_t alpha, int align)
{
	if (alpha == 0)
		return;
	auto font = TTF_OpenFont(fontname.c_str(), size);
	if (!font) return;
	SDL_Color c = { 255, 255, 255, 128 };
	auto text_s = TTF_RenderUTF8_Blended(font, text.c_str(), c);
	auto text_t = SDL_CreateTextureFromSurface(_ren, text_s);
	SDL_SetTextureAlphaMod(text_t, alpha);
	SDL_Rect rect;
	rect.h = text_s->h;
	rect.w = text_s->w;
	rect.y = y;
	switch (align)
	{
	case BP_ALIGN_LEFT:
		rect.x = x;
		break;
	case BP_ALIGN_RIGHT:
		rect.x = x - rect.w;
		break;
	case BP_ALIGN_MIDDLE:
		rect.x = x - rect.w / 2;
		break;
	}

	SDL_RenderCopy(_ren, text_t, nullptr, &rect);
	SDL_DestroyTexture(text_t);
	SDL_FreeSurface(text_s);
	TTF_CloseFont(font);
}

int BigPotEngine::init()
{
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		return -1;
	}
	_win = SDL_CreateWindow("BigPotPlayer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 150, SDL_WINDOW_RESIZABLE);
	_ren = SDL_CreateRenderer(_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE/*| SDL_RENDERER_PRESENTVSYNC*/);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	_logo = loadImage("logo.png");
	showLogo();
	renderPresent();
	TTF_Init();
	return 0;
}

void BigPotEngine::toggleFullscreen()
{
	_full_screen = !_full_screen;
	if (_full_screen)
		SDL_SetWindowFullscreen(_win, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(_win, 0);
	SDL_RenderClear(_ren);	
}

bool BigPotEngine::setKeepRatio(bool b)
{
	return _keep_ratio = b;
}

void BigPotEngine::setPresentPosition()
{
	if (!_tex)
		return;
	int w_dst = 0, h_dst = 0;
	int w_src = 0, h_src = 0;
	getWindowSize(w_dst, h_dst);
	SDL_QueryTexture(_tex, nullptr, nullptr, &w_src, &h_src);
	if (_keep_ratio)
	{
		if (w_src == 0 || h_src == 0) return;
		double w_ratio = 1.0*w_dst / w_src;
		double h_ratio = 1.0*h_dst / h_src;
		double ratio = min(w_ratio, h_ratio);
		if (w_ratio > h_ratio)
		{
			//宽度大，左右留空
			_rect.x = (w_dst - w_src * ratio) / 2;
			_rect.y = 0;
			_rect.w = w_src * ratio;
			_rect.h = h_dst;
		}
		else
		{
			//高度大，上下留空
			_rect.x = 0;
			_rect.y = (h_dst - h_src * ratio) / 2;
			_rect.w = w_dst;
			_rect.h = h_src * ratio;
		}
	}
	else
	{
		_rect.x = 0;
		_rect.y = 0;
		_rect.w = w_dst;
		_rect.h = h_dst;
	}
}



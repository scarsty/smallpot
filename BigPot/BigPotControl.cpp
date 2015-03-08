#include "BigPotControl.h"

BigPotControl BigPotControl::_control;

BigPotControl::BigPotControl()
{
	_this = &_control;
	if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		return;
	}
	win = SDL_CreateWindow("BigPotPlayer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 300, SDL_WINDOW_RESIZABLE);
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	SDL_RenderPresent(ren);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	TTF_Init();
}


BigPotControl::~BigPotControl()
{
	//SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
}

void BigPotControl::renderCopy(BP_Texture* t, int x, int y, int w, int h)
{
	SDL_Rect r = { x, y, w, h };
	SDL_RenderCopy(ren, t, nullptr, &r);
}

int BigPotControl::openAudio(int& freq, int& channels, int& size, int minsize, AudioCallback f)
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

	callback = f;
	//if (useMap())
	{
		want.samples = max(size, minsize);
	}

	device = 0;
	while (device == 0)
	{
		device = SDL_OpenAudioDevice(NULL, 0, &want, &spec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
		want.channels--;
	}
	printf("device %d / %d\n", spec.freq, spec.channels);

	if (device)
	{
		SDL_PauseAudioDevice(device, 0);
	}
	else
	{
		printf("failed to open audio: %s\n", SDL_GetError());
	}

	freq = spec.freq;
	channels = spec.channels;

	return 0;
}

void BigPotControl::mixAudioCallback(void* userdata, Uint8* stream, int len)
{
	SDL_memset(stream, 0, len);
	if (_control.callback)
	{
		_control.callback(stream, len);
	}
}

BP_Texture* BigPotControl::createSquareTexture()
{
	int d = 10;
	auto square_s = SDL_CreateRGBSurface(0, d, d, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
	SDL_FillRect(square_s, nullptr, 0xffffffff);
	SDL_Rect r = { 0, 0, 1, 1 };
	/*auto &x = r.x;
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
	square = SDL_CreateTextureFromSurface(ren, square_s);
	SDL_SetTextureBlendMode(square, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(square, 128);
	SDL_FreeSurface(square_s);
	return square;
}

void BigPotControl::drawText(const string &fontname, const string &text, int size, int x, int y, uint8_t alpha, int align)
{
	if (alpha == 0)
		return;
	auto font = TTF_OpenFont(fontname.c_str(), size);
	if (!font) return;
	SDL_Color c = { 255, 255, 255, 128 };
	auto text_s = TTF_RenderUTF8_Blended(font, text.c_str(), c);
	auto text_t = SDL_CreateTextureFromSurface(ren, text_s);
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

	SDL_RenderCopy(ren, text_t, nullptr, &rect);
	SDL_DestroyTexture(text_t);
	SDL_FreeSurface(text_s);
	TTF_CloseFont(font);
}


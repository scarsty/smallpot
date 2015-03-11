#include "BigPotSubtitle.h"


BigPotSubtitle::BigPotSubtitle()
{
}


BigPotSubtitle::~BigPotSubtitle()
{
}

void BigPotSubtitle::init()
{
	_lib=ass_library_init();


	_ren = ass_renderer_init(_lib);

	ass_set_frame_size(_ren, 1000, 600);

	ass_set_fonts(_ren, "c:\\windows\\fonts\\msyh.ttf", "Sans",0,"",0);


	//ass_fonts_update(_ren);
	_track = ass_read_file(_lib, "1.ass", NULL);
	int a;
	//_img = ass_render_frame(_ren, _track, 40000, &a);
	_img = ass_render_frame(_ren, _track, 40000, &a);
	
	cout << _img->color << endl;
	cout << engine_->getTicks();
	uint32_t *buffer = (uint32_t *)malloc(_img->stride*_img->h * 4);
	for (int x = 0; x < _img->stride; x++)
		for (int y = 0; y < _img->h; y++)
		{
			buffer[y*_img->stride + x] = _img->bitmap[y*_img->stride + x] * 0x01010101;
		}
	cout << engine_->getTicks();
	auto s = SDL_CreateRGBSurfaceFrom(buffer, _img->stride, _img->h, 32, _img->stride * 4, RMASK, GMASK, BMASK, AMASK);

	//auto t = SDL_CreateTexture(engine_->getRenderer(), SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, _img->w,		_img->h);
	auto t = SDL_CreateTextureFromSurface(engine_->getRenderer(), s);
	
	
	//a = SDL_UpdateTexture(t, NULL, _img->bitmap, _img->stride);
	//cout << SDL_GetError();
	engine_->renderCopy(t, 0,0, _img->w, _img->h);
	engine_->renderPresent();

	BP_Event e;
	for (int i = 0;i<10000;i++, engine_->pollEvent(e))
	engine_->delay(1);
}

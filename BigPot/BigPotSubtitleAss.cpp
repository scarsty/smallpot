#include "BigPotSubtitleAss.h"


BigPotSubtitleAss::BigPotSubtitleAss()
{
	init();
}


BigPotSubtitleAss::~BigPotSubtitleAss()
{
	destroyAllTex();
	closeSubtitle();
	destroy();
}

void BigPotSubtitleAss::init()
{
	_lib = ass_library_init();
	_ren = ass_renderer_init(_lib);
	ass_set_fonts(_ren, "/library/fonts/Kaiti.ttc", "Sans", 0, "", 0);
}

bool BigPotSubtitleAss::openSubtitle(const string& filename)
{
	//函数的参数是char*,为免意外复制一份
	auto s = filename;
	//if (checkFileExt(filename))
		_track = ass_read_file(_lib, (char*)s.c_str(), NULL);
	haveSubtitle_ = (_track != nullptr);
	if (haveSubtitle_) subfilename_ = filename;
	return 	haveSubtitle_;
}

void BigPotSubtitleAss::show(int time)
{
	int a;
	_img = ass_render_frame(_ren, _track, time, &a);
	//cout << engine_->getTicks() << endl;
	auto img = _img;
	if (a)
	{
		destroyAllTex();
		while (img)
		{
			auto t = engine_->transBitmapToTexture(img->bitmap, img->color, img->w, img->h, img->stride);
			engine_->renderCopy(t, img->dst_x, img->dst_y, img->w, img->h, 1);
			_tex_v.push_back(t);
			img = img->next;
		}
	}
	else
	{
		int i = 0;
		while (img && i < _tex_v.size())
		{
			engine_->renderCopy(_tex_v[i++], img->dst_x, img->dst_y, img->w, img->h, 1);
			img = img->next;
		}
	}
	//cout << engine_->getTicks() << endl;
}

void BigPotSubtitleAss::destroy()
{
	ass_renderer_done(_ren);
	ass_library_done(_lib);
}

void BigPotSubtitleAss::setFrameSize(int w, int h)
{
	//if (_track)
	ass_set_frame_size(_ren, w, h);
}

void BigPotSubtitleAss::closeSubtitle()
{
	if (_track)
		ass_free_track(_track);
	_track = nullptr;
}

void BigPotSubtitleAss::destroyAllTex()
{
	for (auto t : _tex_v)
	{
		engine_->destroyTexture(t);
	}
	_tex_v.clear();
}
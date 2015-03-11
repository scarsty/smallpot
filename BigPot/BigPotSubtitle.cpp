#include "BigPotSubtitle.h"


BigPotSubtitle::BigPotSubtitle()
{

}


BigPotSubtitle::~BigPotSubtitle()
{
}

void BigPotSubtitle::init()
{
	_lib = ass_library_init();
	_ren = ass_renderer_init(_lib);
	ass_set_fonts(_ren, "c:\\windows\\fonts\\msyh.ttf", "Sans", 0, "", 0);
}

void BigPotSubtitle::openSubtitle(const string& filename)
{
	//函数的参数是char*,为免意外复制一份
	auto s = filename;
	_track = ass_read_file(_lib, (char*)s.c_str(), NULL);
}

void BigPotSubtitle::show(int time)
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

void BigPotSubtitle::destroy()
{

}

void BigPotSubtitle::setFrameSize(int w, int h)
{
	if (_track)
		ass_set_frame_size(_ren, w, h);
}

bool BigPotSubtitle::tryOpenSubtitle(const string& filename)
{
	string str;
	bool b = true;
	do
	{
		str = changeFileExt(filename, "srt");
		if (fileExist(str)) break;
		str = changeFileExt(filename, "ssa");
		if (fileExist(str)) break;
		str = changeFileExt(filename, "ass");
		if (fileExist(str)) break;
		b = false;
	} while (false);

	_haveSubtitle = b;
	if (b)
	{
		openSubtitle(str);
		printf("found subtitle file %s\n", str.c_str());
	}
	else
	{
		printf("no subtitle file\n");
	}
	return b;
}

void BigPotSubtitle::destroyAllTex()
{
	for (auto t : _tex_v)
	{
		engine_->destroyTexture(t);
	}
	_tex_v.clear();
}

#pragma once
#include "BigPotBase.h"

extern "C"
{
#include "ass/ass.h"
#include "ass/ass_types.h"
}

#include<vector>

class BigPotSubtitle :
	public BigPotBase
{
public:
	BigPotSubtitle();
	virtual ~BigPotSubtitle();

private:
	ASS_Library* _lib;
	ASS_Renderer* _ren;
	ASS_Track* _track;
	ASS_Image* _img;

	string _subfilename;
	vector<BP_Texture*> _tex_v;
	int _tex_num = 0;

	bool _haveSubtitle = false;

	void destroyAllTex();

public:
	void init();

	bool exist() { return _haveSubtitle; };
	bool openSubtitle(const string& filename);
	bool reOpenSubtitle() { return openSubtitle(_subfilename); };
	void closeSubtitle();
	void show(int time);
	void destroy();
	void setFrameSize(int w, int h);
	bool tryOpenSubtitle(const string& filename);
};


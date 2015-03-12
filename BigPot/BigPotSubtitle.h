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
	ASS_Library* _lib = nullptr;
	ASS_Renderer* _ren = nullptr;
	ASS_Track* _track = nullptr;
	ASS_Image* _img = nullptr;

	string _subfilename;
	vector<BP_Texture*> _tex_v;
	int _tex_num = 0;

	bool _haveSubtitle = false;

	vector<string> _ext;

	void destroyAllTex();

public:
	void init();

	virtual bool exist() { return _haveSubtitle; };
	virtual bool openSubtitle(const string& filename);
	virtual bool reOpenSubtitle() { return openSubtitle(_subfilename); };
	virtual void closeSubtitle();
	virtual void show(int time);
	virtual void destroy();
	virtual void setFrameSize(int w, int h);
	virtual bool tryOpenSubtitle(const string& filename);
	virtual bool checkFileExt(const string& filename);
};


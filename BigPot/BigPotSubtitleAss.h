#pragma once
#include "BigPotSubtitle.h"

class BigPotSubtitleAss :
	public BigPotSubtitle
{
public:
	BigPotSubtitleAss();
	virtual ~BigPotSubtitleAss();
private:
	ASS_Library* _lib = nullptr;
	ASS_Renderer* _ren = nullptr;
	ASS_Track* _track = nullptr;
	ASS_Image* _img = nullptr;

	vector<BP_Texture*> _tex_v;
	void destroyAllTex();

public:
	virtual void init();
	virtual void destroy();
	virtual bool openSubtitle(const string& filename);
	virtual void closeSubtitle();
	virtual void show(int time);
	virtual void setFrameSize(int w, int h);
};


#pragma once
#include "BigPotBase.h"

extern "C"
{
#include "ass/ass.h"
#include "ass/ass_types.h"
}

#include <vector>

#include "BigPotString.h"
using namespace BigPotString;

enum BigPotPSubtitleType
{
	BPSUB_TYPE_ASS,
	BPSUB_TYPE_SRT
};

class BigPotSubtitle :
	public BigPotBase
{
public:
	BigPotSubtitle();
	virtual ~BigPotSubtitle();
protected:
	BigPotPSubtitleType type_;
	string subfilename_, fontname_;
	bool haveSubtitle_ = false;
public:
	bool exist() { return haveSubtitle_; };
	bool reOpenSubtitle() { return openSubtitle(subfilename_); };

	virtual void init(){};
	virtual bool openSubtitle(const string& filename){ return false; };
	virtual void closeSubtitle(){};
	virtual void show(int time){};
	virtual void destroy(){};
	virtual void setFrameSize(int w, int h){};
};



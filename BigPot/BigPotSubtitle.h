#pragma once
#include "BigPotBase.h"

extern "C"
{
#include "ass/ass.h"
#include "ass/ass_types.h"
}

#include<vector>

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
	string subfilename_;
	bool haveSubtitle_ = false;
	vector<string> _ext;
public:
	bool exist() { return haveSubtitle_; };
	bool reOpenSubtitle() { return openSubtitle(subfilename_); };
	bool tryOpenSubtitle(const string& filename);
	bool checkFileExt(const string& filename);

	virtual void init();
	virtual bool openSubtitle(const string& filename);
	virtual void closeSubtitle();
	virtual void show(int time);
	virtual void destroy();
	virtual void setFrameSize(int w, int h);
};

#include "BigPotSubtitleAss.h"
#include "BigPotSubtitleSrt.h"

class BigPotSubtitleFactory : BigPotBase
{
public:
	BigPotSubtitleFactory();
	~BigPotSubtitleFactory();
	
	BigPotSubtitle* createSubtitle(const string& filename)
	{
		BigPotSubtitle* ret;
		auto ext =toLowerCase(getFileExt(filename));
		if (ext == "ass" || ext=="ssa")
			ret = new BigPotSubtitleAss;
		else if (ext == "srt" || ext == "txt")
			ret = new BigPotSubtitleSrt;
		ret->openSubtitle(filename);
		return ret;
	}

};
#pragma once

#include "BigPotBase.h"
#include "BigPotSubtitleAss.h"
#include "BigPotSubtitleSrt.h"

class BigPotSubtitleFactory : BigPotBase
{
public:
	BigPotSubtitleFactory();
	virtual ~BigPotSubtitleFactory();
private:
	vector<string> _ext;

public:
	BigPotSubtitle* createSubtitle(const string& filename);
	void destroySubtitle(BigPotSubtitle*& sub) { delete sub; sub = nullptr; }
	string lookForSubtitle(const string& filename);

	bool tryOpenSubtitle(const string& filename);
	bool isSubtitle(const string& filename);

};


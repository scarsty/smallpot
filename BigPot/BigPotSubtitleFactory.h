#pragma once

#include "BigPotBase.h"
#include "BigPotSubtitleAss.h"
#include "BigPotSubtitleSrt.h"
#include "BigPotString.h"

using namespace BigPotString;

class BigPotSubtitleFactory : BigPotBase
{
public:
	BigPotSubtitleFactory();
	virtual ~BigPotSubtitleFactory();
private:
	std::vector<std::string> _ext;

public:
	BigPotSubtitle* createSubtitle(const std::string& filename);
	void destroySubtitle(BigPotSubtitle*& sub) { delete sub; sub = nullptr; }
	std::string lookForSubtitle(const std::string& filename);

	bool tryOpenSubtitle(const std::string& filename);
	bool isSubtitle(const std::string& filename);

};


#include "BigPotSubtitle.h"


BigPotSubtitle::BigPotSubtitle()
{
	_ext.push_back("ass");
	_ext.push_back("ssa");
	_ext.push_back("srt");
	_ext.push_back("txt");
}


BigPotSubtitle::~BigPotSubtitle()
{
}


bool BigPotSubtitle::tryOpenSubtitle(const string& filename)
{
	string str;
	bool b = false;

	for (auto &ext : _ext)
	{
		str = changeFileExt(filename, ext);
		if (fileExist(str))
		{
			b = true;
			break;
		}
	}
	if (!b)
	{
		str = fingFileWithMainName(filename);
		if (str != "") b = true;
	}

	if (str != "")
	{
		b = openSubtitle(str);
		printf("try load subtitle file %s, state %d\n", str.c_str(), b);

	}
	return b;
}

bool BigPotSubtitle::checkFileExt(const string& filename)
{
	auto ext = getFileExt(filename);
	transform(ext.begin(), ext.end(), ext.begin(), tolower);
	bool b = false;
	for (auto &e : _ext)
	{
		if (e==ext)
		{
			b = true;
			break;
		}
	}
	return b;
}

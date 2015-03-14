#include "BigPotSubtitleSrt.h"

BigPotSubtitleSrt::BigPotSubtitleSrt()
{
}


BigPotSubtitleSrt::~BigPotSubtitleSrt()
{
}

bool BigPotSubtitleSrt::openSubtitle(const string& filename)
{
	haveSubtitle_ = true;
	_file = fopen(filename.c_str(), "r");
	if (!_file)
	{
		_file = NULL;
		return false;
	}
	else
	{
		//BigPotSubtitleAtom bigpottmp;
		while (!feof(_file))
		{
			readIndex();
		}
		//fscanf("%d")
		return true;
	}
	fclose(_file);
}

void BigPotSubtitleSrt::show(int time)
{
	for (int i = 0; i < _AtomList.size(); i++)
	{
		auto tmplist = _AtomList[i];
		if (tmplist.begintime <= time && tmplist.endtime >= time)
		{
			//engine_->renderCopy()
			int w, h;
			engine_->getWindowSize(w, h);
			engine_->drawSubtitle("c:/windows/fonts/msyh.ttf", tmplist.str, w * 16 / 720, w / 2, h * 9 / 10 - 5, 100, 1);
			break;
		}
	}
}

int BigPotSubtitleSrt::readIndex()
{
	if (!_file)
		return 0;
	if (feof(_file))
		return 0;
	int tmpid = -1;
	fscanf(_file, "%d\n", &tmpid);
	if (tmpid != -1)
	{
		BigPotSubtitleAtom pot;
		return readTime(pot);
	}
	else
	{
		return readIndex();
	}
}

int BigPotSubtitleSrt::readTime(BigPotSubtitleAtom& pot)
{
	if (!_file)return 0;
	if (feof(_file))return 0;
	int btimeh, btimem, btimes, btimems;
	int etimeh, etimem, etimes, etimems;
	if (fscanf(_file, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n",
		&btimeh, &btimem, &btimes, &btimems,
		&etimeh, &etimem, &etimes, &etimems) == 8)
	{
		int totalbegintime = btimems + 1000 * btimes + 1000 * 60 * btimem + 1000 * 3600 * btimeh;
		int totalendtime = etimems + 1000 * etimes + 1000 * 60 * etimem + 1000 * 3600 * etimeh;
		pot.begintime = totalbegintime;
		pot.endtime = totalendtime;
		return readString(pot);
	}
	else
		return -2;
}

int BigPotSubtitleSrt::readString(BigPotSubtitleAtom& pot)
{
	if (!_file)
		return 0;
	if (feof(_file))
		return 0;
	string tmpstr = "";
	while (!feof(_file))
	{
		char tmp[4096] = { 0 };
		fgets(tmp, 4096, _file);
		if (strcmp(tmp, "\n") != 0)
			tmpstr += tmp;
		else
			break;
	}

	pot.str = tmpstr;
	_AtomList.push_back(pot);
	return 0;
}

void BigPotSubtitleSrt::closeSubtitle()
{
	//throw std::logic_error("The method or operation is not implemented.");
}


/*void BigPotSubtitleSrt::tryOpenSubtitle(string open_filename)
{
	openSubtitle(open_filename);
	//throw std::logic_error("The method or operation is not implemented.");
}*/
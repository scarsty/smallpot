#pragma once
#include "BigPotSubtitle.h"
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
typedef struct BigPotSubtitleAtom
{
	int begintime;
	int endtime;
	string str;
}BigPotSubtitleAtom;
class BigPotSubtitleSrt :
	public BigPotSubtitle
{
private:
	vector<BigPotSubtitleAtom> AtomList;
	FILE* file;
	int readIndex();
	int readTime(BigPotSubtitleAtom* pot);
	int readString(BigPotSubtitleAtom* pot);
public:
	BigPotSubtitleSrt();
	virtual ~BigPotSubtitleSrt();
	bool openSubtitle(const string& filename);
	void show(int time);
	bool exist();
	void closeSubtitle();
	void setFrameSize(int w, int h);
	void tryOpenSubtitle(string open_filename);
	void init();
	void destroy();
};


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

	int _fram_w, _frame_h;
public:
	BigPotSubtitleSrt();
	virtual ~BigPotSubtitleSrt();

	//virtual void init();
	//virtual void destroy();
	virtual bool openSubtitle(const string& filename);
	virtual void closeSubtitle();
	virtual void show(int time);
	virtual void setFrameSize(int w, int h){ _fram_w = w; _frame_h = h; }

	//该类比较简单，init和destroy可以为空

	/*bool openSubtitle(const string& filename);
	void show(int time);
	bool exist();
	void closeSubtitle();
	void setFrameSize(int w, int h);
	void tryOpenSubtitle(string open_filename);
	void init();
	void destroy();*/
};


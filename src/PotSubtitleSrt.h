#pragma once
#include "PotSubtitle.h"
#include <stdio.h>
#include <string>
#include <vector>

typedef struct PotSubtitleAtom
{
    int begintime;
    int endtime;
    std::string str;
} PotSubtitleAtom;

class PotSubtitleSrt :
    public PotSubtitle
{
private:
    std::vector<PotSubtitleAtom> _AtomList;
    FILE* _file;
    int readIndex();
    int readTime(PotSubtitleAtom& pot);
    int readString(PotSubtitleAtom& pot);

    int _fram_w, _frame_h;
public:
    PotSubtitleSrt();
    virtual ~PotSubtitleSrt();

    //virtual void init();
    //virtual void destroy();
    virtual bool openSubtitle(const std::string& filename);
    virtual void closeSubtitle();
    virtual void show(int time);
    virtual void setFrameSize(int w, int h) { _fram_w = w; _frame_h = h; }

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


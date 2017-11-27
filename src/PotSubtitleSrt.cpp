#include "PotSubtitleSrt.h"

PotSubtitleSrt::PotSubtitleSrt()
{
}


PotSubtitleSrt::~PotSubtitleSrt()
{
}

bool PotSubtitleSrt::openSubtitle(const std::string& filename)
{
    haveSubtitle_ = true;
    file_ = fopen(filename.c_str(), "r");
    if (!file_)
    {
        file_ = NULL;
        return false;
    }
    else
    {
        //BigPotSubtitleAtom bigpottmp;
        while (!feof(file_))
        {
            readIndex();
        }
        //fscanf("%d")
        return true;
    }
    fclose(file_);
}

void PotSubtitleSrt::show(int time)
{
    for (int i = 0; i < atom_list_.size(); i++)
    {
        auto tmplist = atom_list_[i];
        if (tmplist.begintime <= time && tmplist.endtime >= time)
        {
            //engine_->renderCopy()
            int w, h;
            engine_->getWindowSize(w, h);
            engine_->drawSubtitle(fontname_, tmplist.str, w * 16 / 720, w / 2, h * 9 / 10 - 5, 100, 1);
            break;
        }
    }
}

int PotSubtitleSrt::readIndex()
{
    if (!file_) { return 0; }
    if (feof(file_)) { return 0; }
    int tmpid = -1;
    fscanf(file_, "%d\n", &tmpid);
    if (tmpid != -1)
    {
        PotSubtitleAtom pot;
        return readTime(pot);
    }
    else
    {
        return readIndex();
    }
}

int PotSubtitleSrt::readTime(PotSubtitleAtom& pot)
{
    if (!file_) { return 0; }
    if (feof(file_)) { return 0; }
    int btimeh, btimem, btimes, btimems;
    int etimeh, etimem, etimes, etimems;
    if (fscanf(file_, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n",
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
    {
        return -2;
    }
}

int PotSubtitleSrt::readString(PotSubtitleAtom& pot)
{
    if (!file_)
    {
        return 0;
    }
    if (feof(file_))
    {
        return 0;
    }
    std::string tmpstr = "";
    while (!feof(file_))
    {
        char tmp[4096] = { 0 };
        fgets(tmp, 4096, file_);
        if (strcmp(tmp, "\n") != 0)
        {
            tmpstr += tmp;
        }
        else
        {
            break;
        }
    }

    pot.str = tmpstr;
    atom_list_.push_back(pot);
    return 0;
}

void PotSubtitleSrt::closeSubtitle()
{
    //throw std::logic_error("The method or operation is not implemented.");
}


/*void BigPotSubtitleSrt::tryOpenSubtitle(string open_filename)
{
    openSubtitle(open_filename);
    //throw std::logic_error("The method or operation is not implemented.");
}*/
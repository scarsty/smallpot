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
	file = fopen(filename.c_str(), "r");
	if (!file){
		file = NULL;
		return false;
	}
	else{
		//BigPotSubtitleAtom bigpottmp;
		while (!feof(file)){
			readIndex();
		}
		//fscanf("%d")
		return true;
	}
	fclose(file);
}

void BigPotSubtitleSrt::show(int time){
	for (int i = 0; i < AtomList.size(); i++){
		auto tmplist = AtomList[i];
		if (tmplist.begintime >= time && tmplist.endtime <= time){
			//engine_->renderCopy()
			engine_->drawText("c:/windows/fonts/cambriai.ttf", tmplist.str, 18, 18, 18, 100, 0); 
			break;
		}
	}
}
int BigPotSubtitleSrt::readIndex()
{
	if (file)return 0;
	if (feof(file))return 0;
	BigPotSubtitleAtom* pot = new BigPotSubtitleAtom;
	int tmpid = -1;
	fscanf(file, "%d\n", &tmpid);
	if (tmpid != -1){
		return readTime(pot);
	}
	else{
		return readIndex();
	}
}

int BigPotSubtitleSrt::readTime(BigPotSubtitleAtom* pot)
{
	if (feof(file))return 0;
	int btimeh, btimem, btimes, btimems;
	int etimeh, etimem, etimes, etimems;
	if (fscanf(file, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n",
		&btimeh, &btimem, &btimes, &btimems,
		&etimeh, &etimem, &etimes, &etimems) == 8){
		int totalbegintime = btimems + 1000 * btimes + 1000 * 60 * btimem + 1000 * 3600 * btimeh;
		int totalendtime = etimems + 1000 * etimes + 1000 * 60 * etimem + 1000 * 3600 * etimeh;
		pot->endtime = totalendtime;
		pot->begintime = totalbegintime;
		return readString(pot);
	}
	else
		return -2;
}

int BigPotSubtitleSrt::readString(BigPotSubtitleAtom* pot)
{
	if (feof(file))return 0;
	string tmpstr = "";
	for (;;){
		char tmp[4096] = { 0 };
		//if ()
		if(strcmp(fgets(tmp,4096,file),"\n") != 0)
		//if (fscanf(file,"%s\n", tmp))
			tmpstr += tmp;
		else
			break;
	}
	pot->str = tmpstr;
	AtomList.push_back(*pot);
//	readIndex();
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
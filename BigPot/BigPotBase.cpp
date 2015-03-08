#include "BigPotBase.h"


bool BigPotBase::fileexist(const string& filename)
{
	if (filename.length() <= 0)
		return false;

	fstream file;
	bool ret = false;
	file.open(filename.c_str(), ios::in);
	if (file)
	{
		ret = true;
		file.close();
	}
	return ret;
}

string BigPotBase::readStringFromFile(const string& filename)
{
	FILE *fp = fopen(filename.c_str(), "rb");
	if (fp == nullptr)
	{
		printf("Can not open file %s\n", filename.c_str());
		return "";
	}
	fseek(fp, 0, SEEK_END);
	int length = ftell(fp);
	fseek(fp, 0, 0);
	char* s = new char[length + 1];
	for (int i = 0; i <= length; s[i++] = '\0');
	fread(s, length, 1, fp);
	string str(s);
	fclose(fp);
	delete[] s;
	return str;
}

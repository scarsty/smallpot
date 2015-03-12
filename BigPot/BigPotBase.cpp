#include "BigPotBase.h"

string BigPotBase::filepath_;

bool BigPotBase::fileExist(const string& filename)
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

string BigPotBase::getFileExt(const string& filename)
{
	int pos_p = filename.find_last_of(_path_);
	int pos_d = filename.find_last_of('.');
	return filename.substr(pos_d + 1);
	return "";
}

string BigPotBase::getFileMainname(const string& filename)
{
	int pos_p = filename.find_last_of(_path_);
	int pos_d = filename.find_last_of('.');
	if (pos_p < pos_d)
		return filename.substr(0, pos_d);
	return filename;
}

std::string BigPotBase::changeFileExt(const string& filename, const string& ext)
{
	auto e = ext;
	if (e != "" && e[0] != '.')
		e = "." + e;
	return getFileMainname(filename) + e;
}

std::string BigPotBase::getFilePath(const string& filename)
{
	int pos_p = filename.find_last_of(_path_);
	if (pos_p != string::npos)
		return filename.substr(0, pos_p);
	return "";
}

//查找相似文件名的文件，只找两个
std::string BigPotBase::fingFileWithMainName(const string& filename)
{
	_finddata_t file;
	long fileHandle;
	string path = getFilePath(filename);
	string ext = getFileExt(filename);
	if (path != "") path = path + _path_;
	string findname = getFileMainname(filename) + ".*";
	string ret = "";
	fileHandle = _findfirst(findname.c_str(), &file);
	ret = path + file.name;
	if (getFileExt(ret) == ext)
	{
		_findnext(fileHandle, &file);
		ret = path + file.name;
		if (getFileExt(ret) == ext)
			ret = "";
	}
	_findclose(fileHandle);
	return ret;
}

string BigPotBase::toLowerCase(const string& str)
{
	auto str1 = str;
	transform(str1.begin(), str1.end(), str1.begin(), tolower); 
	return str1;
}

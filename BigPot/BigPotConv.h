#pragma once

extern "C"
{
#ifdef _MSC_VER
#include "libiconv/iconv.h"
#else
#include <iconv.h>
#endif
}

#include <cstring>
#include <string>
#include <algorithm>

using namespace std;

#define CONV_BUFFER_SIZE 2048

class BigPotConv
{
public:
	BigPotConv();
	virtual ~BigPotConv();

	string toLowerCase(const string s);

	static string conv(const string& src, const char* from, const char* to);
	static string conv(const string& src, const string& from, const string& to)
	{
		return conv(src, from.c_str(), to.c_str());
	}
	static string cp936toutf8(const string& src) { return conv(src, "cp936", "utf-8"); }
	static string cp950toutf8(const string& src) { return conv(src, "cp950", "utf-8"); }
};


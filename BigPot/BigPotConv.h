#pragma once

extern "C"
{
#include "libiconv/iconv.h"
}

#include <string>
#include <algorithm>

using namespace std;

#define CONV_BUFFER_SIZE 2048

class BigPotConv
{
public:
	BigPotConv();
	virtual ~BigPotConv();

	static string conv(const string& src, const char* from, const char* to);
	static string cp936toutf8(const string& src) { return conv(src, "cp936", "utf-8"); }
	static string cp950toutf8(const string& src) { return conv(src, "cp950", "utf-8"); }
};


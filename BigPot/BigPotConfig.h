#pragma once

#include "BigPotBase.h"
#include "json/json.h"
#include <stdint.h>

class BigPotConfig : public BigPotBase
{
private:
	Json::Value value;
	string content;
public:
	BigPotConfig();
	virtual ~BigPotConfig();

	void init();
	void write();
	void getInteger(int &v, const char * name)
	{
		if (value[name].isInt())
			v = value[name].asInt();
	}
	void getString(string &v, const char * name)
	{
		if (value[name].isString())
			v = value[name].asString();
		cout << value[name].asString();
	}
	void getDouble(double &v, const char * name)
	{
		if (value[name].isDouble())
			v = value[name].asDouble();
	}
	void getBool(bool &v, const char * name)
	{
		if (value[name].isBool())
			v = value[name].asBool();
	}
	void setInteger(int v, const char * name) { value[name] = v; }
	void setString(const string v, const char * name) { value[name] = v; }
	void setDouble(double v, const char * name) { value[name] = v; }
	void setBool(bool v, const char * name) { value[name] = v; }
};


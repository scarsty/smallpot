#pragma once

#include "BigPotBase.h"
#include "json/json.h"
#include <stdint.h>
#include <direct.h>

class BigPotConfig : public BigPotBase
{
private:
	Json::Value _value, _record;
	string _content;
	string _pathname;
	Json::Value& getValue(Json::Value& v)
	{
		return v;
	}

public:
	BigPotConfig();
	virtual ~BigPotConfig();

	void init();
	void write();
	//以下函数注意默认值
	int getInteger(const char * name)
	{
		return _value[name].isInt() ? _value[name].asInt() : 0;
	}
	string getString(const char * name)
	{
		return _value[name].isString() ? _value[name].asString() : "";
	}
	double getDouble(const char * name)
	{
		return _value[name].isDouble() ? _value[name].asDouble() : 0.0;
	}
	bool getBool(bool &v, const char * name)
	{
		return _value[name].isBool() ? _value[name].asBool() : false;
	}
	void setInteger(int v, const char * name) { _value[name] = v; }
	void setString(const string v, const char * name) { _value[name] = v; }
	void setDouble(double v, const char * name) { _value[name] = v; }
	void setBool(bool v, const char * name) { _value[name] = v; }

	int getRecord(const char * name)
	{
		return _record[name].isInt() ? _record[name].asInt() : 0;
	}
	void setRecord(int v, const char * name) { _record[name] = v; }
	void clearRecord() { _record.clear(); }
};


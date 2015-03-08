#pragma once

#include "BigPotBase.h"
#include "json/json.h"
#include <stdint.h>

class BigPotConfig : public BigPotBase
{
private:
	Json::Value _value, _record;
	string _content;
	Json::Value& getValue(Json::Value& v)
	{
		return v;
	}
public:
	BigPotConfig();
	virtual ~BigPotConfig();

	void init();
	void write();
	void getInteger(int &v, const char * name)
	{
		if (_value[name].isInt())
			v = _value[name].asInt();
	}
	void getString(string &v, const char * name)
	{
		if (_value[name].isString())
			v = _value[name].asString();
	}
	void getDouble(double &v, const char * name)
	{
		if (_value[name].isDouble())
			v = _value[name].asDouble();
	}
	void getBool(bool &v, const char * name)
	{
		if (_value[name].isBool())
			v = _value[name].asBool();
	}
	void setInteger(int v, const char * name) { _value[name] = v; }
	void setString(const string v, const char * name) { _value[name] = v; }
	void setDouble(double v, const char * name) { _value[name] = v; }
	void setBool(bool v, const char * name) { _value[name] = v; }

	void getRecord(int &v, const char * name)
	{
		if (_record[name].isInt())
			v = _record[name].asInt();
	}
	void setRecord(int v, const char * name) { _record[name] = v; }
	void clearRecord() { _record.clear(); }
};


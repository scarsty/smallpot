#pragma once

#include "BigPotBase.h"
#include "json/json.h"
#include <stdint.h>

class BigPotConfig : public BigPotBase
{
private:
	Json::Value value, record;
	string content;
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
		if (value[name].isInt())
			v = value[name].asInt();
	}
	void getString(string &v, const char * name)
	{
		if (value[name].isString())
			v = value[name].asString();
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

	void getRecord(int &v, const char * name)
	{
		if (record[name].isInt())
			v = record[name].asInt();
	}
	void setRecord(int v, const char * name) { record[name] = v; }
	void clearRecord() { record.clear(); }
};


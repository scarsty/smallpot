#pragma once

//还是用json算了
//#define USINGJSON

#include "BigPotString.h"

#ifdef USINGJSON
#if (defined _WIN32) || (defined WIN32)
#	ifdef _DEBUG
#		pragma comment(lib,"lib_json_debug.lib")
#	else
#		pragma comment(lib,"lib_json_release.lib")
#	endif
#endif
#endif

#ifdef USINGJSON
#include "json/json.h"
#else
#include "tinyxml2/tinyxml2.h"
#include <stdint.h>
#endif

#include "hash/sha3.h"

class BigPotConfig
{
private:
	static BigPotConfig _config;
	BigPotConfig* _this;
	std::string _content;
	std::string _filename;
	SHA3 _sha3;
	BigPotConfig();
	virtual ~BigPotConfig();
#ifdef USINGJSON
	Json::Value _value, _record;
#else
	tinyxml2::XMLDocument _doc;
	tinyxml2::XMLElement *_root, *_record;

	tinyxml2::XMLElement* getElement(tinyxml2::XMLElement *parent, const char * name);
	

#endif


public:
	void init(const std::string& filepath);
	void write();
	static BigPotConfig* getInstance(){ return &_config; };
	//以下函数注意默认值
#ifdef USINGJSON
	int getInteger(const char * name, int def = 0)
	{
		return _value[name].isInt() ? _value[name].asInt() : def;
	}
	string getString(const char * name, std::string def = "")
	{
		return _value[name].isString() ? _value[name].asString() : def;
	}
	double getDouble(const char * name, double def = 0.0)
	{
		return _value[name].isDouble() ? _value[name].asDouble() : def;
	}
	bool getBool(const char * name, bool def = false)
	{
		return _value[name].isBool() ? _value[name].asBool() : def;
	}
	void setInteger(int v, const char * name) { _value[name] = v; }
	void setString(const std::string v, const char * name) { _value[name] = v; }
	void setDouble(double v, const char * name) { _value[name] = v; }
	void setBool(bool v, const char * name) { _value[name] = v; }

	int getRecord(const char * name)
	{
		auto name_s = _sha3(name);
		if (!_record.isMember(name_s))
		{
			return 0;
		}
		return _record[name_s].isInt() ? _record[name_s].asInt() : 0;
	}
	void setRecord(int v, const char * name) { _record[_sha3(name)] = v; }
	void clearRecord() { _record.clear(); }
#else
	//xml只有字串，故首先完成字串功能
	std::string getString(const char * name, std::string def = "");
	int getInteger(const char * name, int def = 0);
	double getDouble(const char * name, double def = 0.0);
	bool getBool(bool &v, const char * name);

	void setString(const std::string v, const char * name);
	void setInteger(int v, const char * name);
	void setDouble(double v, const char * name);
	void setBool(bool v, const char * name);

	//记录
	int getRecord(const char * name);
	void removeRecord(const char * name);
	void setRecord(int v, const char * name);	
	void clearRecord();	//string replace(string str, const char *string_to_replace, const char *new_string);
#endif
};


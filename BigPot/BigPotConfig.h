#pragma once

#include "BigPotBase.h"
#include "json/json.h"
#include "tinyxml.h"
#include <stdint.h>
#include "ZBase64.h"
//#define USINGJSON

class BigPotConfig : public BigPotBase
{
private:
	string _content;
	string _filename;
#ifdef USINGJSON
	Json::Value _value, _record;
#else
	TiXmlDocument* doc;
	TiXmlElement* tablecontent;
	TiXmlElement* element;
	TiXmlElement* rootElement;
	bool canread;
#endif



public:
	BigPotConfig();
	virtual ~BigPotConfig();

	void init();
	void write();
	//以下函数注意默认值
#ifdef USINGJSON
	int getInteger(const char * name, int def = 0)
	{
		return _value[name].isInt() ? _value[name].asInt() : def;
	}
	string getString(const char * name, string def = "")
	{
		return _value[name].isString() ? _value[name].asString() : def;
	}
	double getDouble(const char * name, double def = 0.0)
	{
		return _value[name].isDouble() ? _value[name].asDouble() : def;
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
		if (!_record.isMember(name))
		{
			return 0;
		}
		return _record[name].isInt() ? _record[name].asInt() : 0;
	}
	void setRecord(int v, const char * name) { _record[name] = v; }
	void clearRecord() { _record.clear(); }
#else
	int getInteger(const char * name, int def = 0)
	{
		if (!canread)return def;
		auto tmpelement = tablecontent->FirstChildElement(name);
		if (tmpelement){
			return atoi(tmpelement->FirstChild()->Value());
		}
		else
			return def;
		//return _value[name].isInt() ? _value[name].asInt() : def;
	}
	string getString(const char * name, string def = "")
	{
		if (!canread)return def;
		auto tmpelement = tablecontent->FirstChildElement(name);
		if (tmpelement){
			return (tmpelement->FirstChild()->Value());
		}
		else
			return def;
		//return _value[name].isString() ? _value[name].asString() : def;
	}
	double getDouble(const char * name, double def = 0.0)
	{
		if (!canread)return def;
		auto tmpelement = tablecontent->FirstChildElement(name);
		if (tmpelement){
			return atof(tmpelement->FirstChild()->Value());
		}
		else
			return def;
		//return _value[name].isDouble() ? _value[name].asDouble() : def;
	}
	bool getBool(bool &v, const char * name)
	{
		//not support
		return false;
		//return _value[name].isBool() ? _value[name].asBool() : false;
	}

	void setInteger(int v, const char * name) {
		char buf[64] = { 0 };
#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF(buf, sizeof(buf), "%d", v);
#else
		sprintf(buf, "%d", _value);
#endif
		if (tablecontent->FirstChildElement(name)){
			tablecontent->FirstChildElement(name)->FirstChild()->SetValue(buf);
		}
		else{
			auto tmpelement = new TiXmlElement(name);
			auto titext = new TiXmlText(buf);
			tmpelement->InsertEndChild(*titext);
			setdata(*tmpelement);
		}
	}


	void setString(const string v, const char * name) {
		if (tablecontent->FirstChildElement(name)){
			//auto titext = new TiXmlText(buf);
			tablecontent->FirstChildElement(name)->FirstChild()->SetValue(v.c_str());
		}
		else{
			auto tmpelement = new TiXmlElement(name);
			auto titext = new TiXmlText(v.c_str());
			tmpelement->InsertEndChild(*titext);
			//tmpelement->SetAttribute(name, v.c_str());
			setdata(*tmpelement);
		}
	}


	void setDouble(double v, const char * name) {
		char buf[64] = { 0 };
#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF(buf, sizeof(buf), "%f", v);
#else
		sprintf(buf, "%d", _value);
#endif
		if (tablecontent->FirstChildElement(name)){
			tablecontent->FirstChildElement(name)->SetValue(buf);
		}
		else{
			auto tmpelement = new TiXmlElement(name);
			auto titext = new TiXmlText(buf);
			tmpelement->InsertEndChild(*titext);
			setdata(*tmpelement);
		}
	}
	void setdata(TiXmlElement& element){
		tablecontent->InsertEndChild(element);
	}
	void setBool(bool v, const char * name) {
		//not support
		//(name, v);
	}
	int getRecord(const char * name)
	{
		if (!canread)return 0;
		auto _record = tablecontent->FirstChildElement("record");
		if (!_record)
		{

			return 0;
		}
		auto tmp = _record->FirstChildElement(ZBase64::Encode((const unsigned char*) name, strlen(name)).c_str());
		if (tmp){
			return atoi(tmp->GetText());
		}
		else
			return 0;
	}
	void setRecord(int v, const char * name) {
		char buf[64] = { 0 };
#if defined(TIXML_SNPRINTF)		
		TIXML_SNPRINTF(buf, sizeof(buf), "%d", v);
#else
		sprintf(buf, "%d", _value);
#endif
		auto _record = tablecontent->FirstChildElement("record");
		if (!_record)
		{
			auto titext = new TiXmlText(buf);
			_record = new TiXmlElement("record");
			auto newti = new TiXmlElement(ZBase64::Encode((const unsigned char*)name,strlen(name)).c_str());
			//auto titext = new TiXmlText(buf);
			newti->InsertEndChild(*titext);
			_record->InsertEndChild(*newti);
			setdata(*_record);
		}
		else{
			_record->FirstChildElement(ZBase64::Encode((const unsigned char*) name, strlen(name)).c_str())->FirstChild()->SetValue(buf);
			//auto newti = new TiXmlElement(ZBase64::Encode((const unsigned char*) name, strlen(name)).c_str());
			//newti->InsertEndChild(*titext);
			//_record->InsertEndChild(*newti);
		}
	}
	void clearRecord() {
		if (!canread)return;
		auto _record = tablecontent->FirstChildElement("record");
		if (_record){
			_record->Clear();
		}
	}
	string replace(string str, const char *string_to_replace, const char *new_string);
#endif
};


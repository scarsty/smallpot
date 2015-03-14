#include "BigPotConfig.h"


BigPotConfig::BigPotConfig()
{
}


BigPotConfig::~BigPotConfig()
{
}

void BigPotConfig::init()
{
	_filename = filepath_ + "/config.json";
	printf("try find config file: %s\n", _filename.c_str());

	
	Json::Reader reader;
	_content = readStringFromFile(_filename);
	reader.parse(_content, _value);

	if (_value["record"].isObject())
		_record = _value["record"];

	_doc.LoadFile(_filename.c_str());

	//_doc.FirstChildElement("volume");
	if (_doc.Error())
		_doc.Clear();
	XMLDeclaration *pDel = _doc.NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
	_doc.NewElement("volume");
	//_doc.
}

void BigPotConfig::write()
{
	Json::StyledWriter writer;

	_value["record"] = _record;
	_content = writer.write(_value);

	ofstream ofs;
	ofs.open(_filename);
	ofs << _content;

	//auto x=_doc.SaveFile(_filename.c_str());
}


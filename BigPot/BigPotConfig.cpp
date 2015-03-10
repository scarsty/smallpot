#include "BigPotConfig.h"


BigPotConfig::BigPotConfig()
{
}


BigPotConfig::~BigPotConfig()
{
}

void BigPotConfig::init()
{
	//value.

	char buf[1024];
	getcwd(buf, 1024);
	_pathname = buf;

	Json::Reader reader;
	_content = readStringFromFile(_pathname + "\\config.json");
	reader.parse(_content, _value);

	if (_value["record"].isObject())
		_record = _value["record"];
}

void BigPotConfig::write()
{
	Json::StyledWriter writer;

	_value["record"] = _record;
	_content = writer.write(_value);

	ofstream ofs;
	ofs.open(_pathname + "\\config.json");
	ofs << _content;
}


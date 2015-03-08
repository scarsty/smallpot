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
	Json::Reader reader;
	content = readStringFromFile("config.json");
	reader.parse(content, value);

	if (value["record"].isObject())
		record = value["record"];
}

void BigPotConfig::write()
{
	Json::StyledWriter writer;

	value["record"] = record;
	content = writer.write(value);

	ofstream ofs;
	ofs.open("config.json");
	ofs << content;
}


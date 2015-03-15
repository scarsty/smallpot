#include "BigPotConfig.h"


BigPotConfig::BigPotConfig()
{
}


BigPotConfig::~BigPotConfig()
{
}

void BigPotConfig::init()
{
#ifdef USINGJSON
	_filename = filepath_ + "/config.json";
	printf("try find config file: %s\n", _filename.c_str());

	
	Json::Reader reader;
	_content = readStringFromFile(_filename);
	reader.parse(_content, _value);

	if (_value["record"].isObject())
		_record = _value["record"];
#else
	_filename = "config.xml";
	printf("try find config file: %s\n", _filename.c_str());
	this->doc = new TiXmlDocument(_filename.c_str());
	if (doc){
		canread = doc->LoadFile(TIXML_ENCODING_UTF8);
		if (canread){
			tablecontent = doc->FirstChildElement("root"); 
		}
		else{
			tablecontent = new TiXmlElement("root");
		}
	}
	else{
		tablecontent = new TiXmlElement("root");
	}
#endif
	//_doc.
}
string BigPotConfig::replace(string str, const char *string_to_replace, const char *new_string)
{
	//查找第一个匹配的字符串
	int index = str.find(string_to_replace);
	// 如果有匹配的字符串
	while (index != std::string::npos)
	{
		// 替换
		str.replace(index, strlen(string_to_replace), new_string);
		// 查找下一个匹配的字符串
		index = str.find(string_to_replace, index + strlen(new_string));
	}
	return str;
}
void BigPotConfig::write()
{
	_filename = "config.xml";
	TiXmlDocument xmlDoc(_filename.c_str()); // 建立一个XML文件

	TiXmlDeclaration Declaration("1.0", "utf-8", "yes"); // 声明XML的属性

	xmlDoc.InsertEndChild(Declaration); // 写入基本的XML头结构
	xmlDoc.InsertEndChild(*tablecontent);
	xmlDoc.Print();
	xmlDoc.SaveFile();
	//auto x=_doc.SaveFile(_filename.c_str());
}


#pragma once
#include <string>
#include "layout.h"


class Util
{
public:
	static std::string toString(ImageScaleType scaleType);
	static std::string toString(Anchor anchor);
	static int readString(uint8_t *buf, std::string &out);
	static uint8_t readInt8(uint8_t *buf, int &out);
	static char readChar(uint8_t *buf, char &out);
	static int readInt32(uint8_t *buf, int &out);
	static int readFloat32(uint8_t *buf, float &out);
	static int readBool(uint8_t *buf, bool &out);
	static std::string getIndent(int level);

	template<class T>
	static void writeXmlElement(std::stringstream &str, int indent, std::string name, T value)
	{
		str << getIndent(indent) << "<" << name << ">" << value << "</" << name << ">" << std::endl;
	}

	template<class T>
	static std::string writeXmlAttribute(std::stringstream &str, std::string attr, T value)
	{
		str << " " << attr << "=\"" << value << "\"";
		return str.str();
	}

private:
	static void enumValueToString(int src, int val, std::string str, std::stringstream &out, bool last = false);
	static std::unordered_map<int, std::string> indentCache;
};

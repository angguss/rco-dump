#include <sstream>
#include "util.h"
#include <iomanip>

std::unordered_map<int, std::string> Util::indentCache;

int Util::readString(uint8_t *buf, std::string &out)
{
	uint8_t len;
	
	// Length is a single byte in the format so each string
	// is capped at 256 bytes
	char strbuf[257];

	memset(strbuf, 0, 257);
	memcpy(&len, buf, sizeof(len));
	buf += sizeof(len);

	if (len > 0 && len < 257)
	{
		memcpy(strbuf, buf, len);
		out = strbuf;
	}
	else
	{
		// Zero-length strings still need assigning
		out = "";
	}

	// Size of the string + the 1 byte at the start
	return len + sizeof(len);
}

char Util::readChar(uint8_t* buf, char& out)
{
	memcpy(&out, buf, sizeof(char));
	return sizeof(char);
}

uint8_t Util::readInt8(uint8_t *buf, int &out)
{
	memcpy(&out, buf, sizeof(uint8_t));
	return sizeof(uint8_t);
}

int Util::readInt32(uint8_t *buf, int &out)
{
	memcpy(&out, buf, sizeof(out));
	return sizeof(out);
}

int Util::readFloat32(uint8_t *buf, float &out)
{
	memcpy(&out, buf, sizeof(out));
	return sizeof(out);
}

int Util::readBool(uint8_t *buf, bool &out)
{
	uint8_t tmp;
	memcpy(&tmp, buf, sizeof(tmp));
	out = tmp > 0;
	return 1;
}

std::string Util::getIndent(int level)
{
	std::string tmp;

	if (indentCache.count(level) > 0)
		return indentCache[level];

	for (int i = 0; i < level; i++)
		tmp += '\t';

	indentCache[level] = tmp;

	return tmp;
}

std::string Util::toString(ImageScaleType scaleType)
{
	switch (scaleType)
	{
	case ScaleAspectInside:
		return "ScaleAspectInside";
	case ScaleCenter:
		return "ScaleCenter";
	case ScaleAspectOutside:
		return "ScaleAspectOutside";
	case ScaleNinePatch:
		return "ScaleNinePatch";
	case ScaleStretch:
		return "ScaleStretch";
	default:
		return "UNSUPPORTED";
	}
}

void Util::enumValueToString(int src, int val, std::string str, std::stringstream& out, bool last)
{
	if (src & val)
	{
		out << str;
		std::string tempString = out.str();
		if (tempString[tempString.length() - 1] != ',' && tempString.length() > 0 && !last)
			out << "|";
	}
}


std::string Util::toString(Anchor anchor)
{
	if (anchor == AnchorNone)
		return "AnchorNone";
	
	std::stringstream str;

	enumValueToString(anchor, AnchorBottom, "AnchorBottom", str);
	enumValueToString(anchor, AnchorHeight, "AnchorHeight", str);
	enumValueToString(anchor, AnchorLeft, "AnchorLeft", str);
	enumValueToString(anchor, AnchorRight, "AnchorRight", str);
	enumValueToString(anchor, AnchorTop, "AnchorTop", str);
	enumValueToString(anchor, AnchorWidth, "AnchorWidth", str, true);

	return str.str();
}

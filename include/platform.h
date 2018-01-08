#pragma once

#include <string>
#include <codecvt>
#include <locale>

#ifdef _WIN32
#include <direct.h>
#else 
#include <sys/types.h>
#include <sys/stat.h>
#endif

inline void createDirectory(std::string dir)
{
#ifdef _WIN32
	_mkdir(dir.data());
#else
	mkdir(dir.data(), 0777);
#endif	
}

inline void writeIndent(FILE *f, int depth)
{
	if (!f) return;
	for (unsigned int i = 0; i < depth; i++)
		fprintf(f, "\t");
}

inline std::string utf16_to_utf8(std::u16string str)
{
#ifdef _WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(str.data());
	return convert.to_bytes(p);
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(str.data());
#endif
}


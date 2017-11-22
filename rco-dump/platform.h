#pragma once

#include <string>

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



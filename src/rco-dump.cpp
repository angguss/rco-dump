// rco-dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS 1

#include <string>
#include <stdio.h>

#include <rco.h>

int main(int argc, char *argv[])
{
	FILE *f;
	std::string file;
	std::string outdir = ".";
	
	if (argc > 1)
		file = argv[1];

	if (argc > 2)
		outdir = argv[2];


	if (file.empty())
		return 1;

	f = fopen(file.c_str(), "rb");
	if (!f)
		return 1;

	// Got an absolute file
	RCO rco(f);
	
	if (file[1] == ':')
		outdir = file.substr(0, file.length() - 4);
	else
		outdir = outdir + "/" + file.substr(0, file.length() - 4);
	rco.dump(outdir);

    return 0;
}


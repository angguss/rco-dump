#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#include <string>
#ifdef __GNUC__
#include <cstring>
#endif
#include <stdio.h>

#include <rco.h>

int main(int argc, char *argv[])
{
	FILE *f;
	std::string file;
	std::string outdir = ".";

	// Loop over arguments and handle accordingly.
	// -fINPUT_FILE
	// -dOUTPUT_DIR
	for (int i = 0; i < argc; i++)
	{
		if (argv[i] != nullptr)
		{
			int arglen = strlen(argv[i]);
			if (arglen > 2)
			{
				if (argv[i][0] == '-' && argv[i][1] == 'f')
				{
					file = &argv[i][2];
				}
				else if (argv[i][0] == '-' && argv[i][2] == 'd')
				{
					outdir = &argv[i][2];
				}
			}
		}
	}

	// If no -f or -d, attempt to parse any >1 args as input files, 
	// allows to keep supporting drag&drop on windows
	if (file.empty())
	{
		if (argc > 1)
			file = argv[1];
		
		if (file.empty())
			return 1;	
	}

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


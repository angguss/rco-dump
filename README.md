# rco-dump

Utility to extract the contents and structure of RCO files from Sony Playstation systems.

## Dependencies

 - zlib

On windows you will need the correct version of zlibstatic. For VS2015 you can this the precompiled version from [here](https://www.nuget.org/packages/zlib-vc140-static-32_64/)

## Building

The build process uses cmake, it should be sufficient to do something like the following:

	mkdir build
	cd build
	cmake ..
	make

## Executing

Run rco-dump on an rco file (or drag & drop)

	rco-dump accessibility_plugin.rco

The program will create a directory with the extracted contents

	accessibility_plugin/
	accessibility_plugin/index.xml
	accessibility_plugin/sounds/*
	accessibility_plugin/textures/*
	accessibility_plugin/xmls/*
	accessibility_plugin/layouts/*



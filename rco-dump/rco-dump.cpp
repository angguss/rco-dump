// rco-dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS 1

#include "stdafx.h"
#include <string>
#include <stdio.h>

#include "platform.h"
#include "rco.h"

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

	RCO rco(f);
	
	outdir = outdir + "/" + file.substr(0, file.length() - 4);
	rco.dump(outdir);


	//struct rco_file rco;

	//fseek(f, 0, SEEK_SET);
	//fread(&rco.header, sizeof(rco_header), 1, f);
	//
	//fseek(f, rco.header.tree_start_off, SEEK_SET);
	//fread(&rco.treetable.root_element, sizeof(rco_tree_table_element), 1, f);
	//rco_tree_table_element_attribute *attributes = new rco_tree_table_element_attribute[rco.treetable.root_element.attr_ct];

	//fread(attributes, sizeof(rco_tree_table_element_attribute) * rco.treetable.root_element.attr_ct, 1, f);

	//fseek(f, rco.header.strings_start_off, SEEK_SET);
	//fread(rco.strings, sizeof(uint8_t), rco.header.strings_size, f);
	//
	//for (int i = 0; i < rco.treetable.root_element.attr_ct; i++)
	//{
	//	if (attributes[i].type == ATTRIBUTE_TYPE::STRING)
	//	{
	//		char *buf = new char[attributes[i].s.len + 1];
	//		memset(buf, 0, attributes[i].s.len + 1);
	//		fseek(f, rco.header.strings_start_off + attributes[i].s.offset, SEEK_SET);
	//		fread(buf, sizeof(char), attributes[i].s.len, f);
	//		std::string tmp = buf;
	//	}
	//}

	//printf("sizeof(rco_tree_table_element) = %d\n", sizeof(rco_tree_table_element));

	//for (int i = 0; i < rco.treetable.root_element.attr_ct; i++)
	//{
	//	fseek(f, rco.treetable.root_element.)
	//}

	//rco.tree = new uint32_t[rco.header.tree_size / sizeof(uint32_t)];
	//printf("%d rco.tree elements\n", rco.header.tree_size / sizeof(uint32_t));
	//
	//rco.id_str = new uint8_t[rco.header.id_str_size];
	//
	//rco.id_int = new uint8_t[rco.header.id_int_size];
	//rco.strings = new uint8_t[rco.header.strings_size];
	//rco.chars = new uint8_t[rco.header.char_size];
	//rco.styles = new uint8_t[rco.header.styles_size];
	//rco.ints_arr = new uint8_t[rco.header.ints_arr_size];
	//rco.float_arr = new double[rco.header.float_arr_size];
	//rco.file_table = new uint8_t[rco.header.file_table_size];
	//
	//fseek(f, rco.header.tree_start_off, SEEK_SET);
	////fread(rco.tree, sizeof(uint32_t), rco.header.tree_size / sizeof(uint32_t), f);
	//fread(&rco.treetable, rco.header.tree_size, sizeof(tree_table), f);
	//
	//
	//fseek(f, rco.header.id_str_start_off, SEEK_SET);
	//fread(rco.id_str, sizeof(uint8_t), rco.header.id_str_size, f);
	//
	//fseek(f, rco.header.id_int_start_off, SEEK_SET);
	//fread(rco.id_int, sizeof(uint8_t), rco.header.id_int_size, f);
	//
	//fseek(f, rco.header.strings_start_off, SEEK_SET);
	//fread(rco.strings, sizeof(uint8_t), rco.header.strings_size, f);
	//
	//fseek(f, rco.header.char_start_off, SEEK_SET);
	//fread(rco.chars, sizeof(uint8_t), rco.header.char_size, f);
	//
	//fseek(f, rco.header.styles_off, SEEK_SET);
	//fread(rco.styles, sizeof(uint8_t), rco.header.styles_size, f);
	//
	//fseek(f, rco.header.ints_arr_off, SEEK_SET);
	//fread(rco.ints_arr, sizeof(uint8_t), rco.header.ints_arr_size, f);
	//
	//fseek(f, rco.header.float_arr_off, SEEK_SET);
	//fread(rco.float_arr, sizeof(double), rco.header.float_arr_size / sizeof(double), f);
	//
	//fseek(f, rco.header.file_table_off, SEEK_SET);
	//fread(rco.file_table, sizeof(uint8_t), rco.header.file_table_size, f);

    return 0;
}


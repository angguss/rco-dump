#pragma once
#include <cstdint>


struct RCO
{
	uint8_t magic[4]; //RCOF
	uint32_t version;
	
	uint32_t tree_start_off;
	uint32_t tree_size;
	
	uint32_t id_str_start_off;
	uint32_t id_str_size;
	
	uint32_t id_int_start_off;
	uint32_t id_int_size;
	
	uint32_t strings_start_off;
	uint32_t strings_size;
	
	uint32_t char_start_off;
	uint32_t char_size;
	
	uint32_t styles_off;
	uint32_t styles_size;

	uint32_t ints_arr_off;
	uint32_t ints_arr_size;

	uint32_t float_arr_off;
	uint32_t float_arr_size;

	uint32_t file_table_off;
	uint32_t file_table_size;


};

#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

struct rco_header
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
typedef enum {
	NONE,
	INTEGER,
	FLOAT,
	STRING,
	CHAR,
	STYLE_ID,
	INTEGER_ARRAY,
	FLOAT_ARRAY,
	DATA,
	ID_STR_LPB, //loopback
	ID_STR,
	ID_INT_LPB, //loopback
	ID_INT,
} ATTRIBUTE_TYPE;

struct string_data_raw {
	int offset;
	int len;
};

struct char_data_raw {
	int offset;
	int len;
};

struct int_array_data_raw {
	int offset;
	int num;
};

struct float_array_data_raw {
	int offset;
	int num;
};

struct file_data_raw {
	int offset;
	int size;
};

struct id_string_table_data_raw {
	int offset;
	bool loopback; // still not sure what this means
};

struct id_int_table_data_raw {
	int offset;
	bool loopback;
};

struct rco_tree_table_element_attribute_raw
{
	uint32_t string_offset;
	uint32_t type;

	union {
		uint32_t i;
		float f;
		string_data_raw s;
		char_data_raw c;
		int_array_data_raw ia;
		float_array_data_raw fa;
		file_data_raw file;
		id_string_table_data_raw id;
		id_int_table_data_raw idref;
	};
};

struct rco_tree_table_element
{
	uint32_t element_offset;
	uint32_t attr_ct;
	uint32_t parent;
	uint32_t previous_sibling;
	uint32_t next_sibling;
	uint32_t first_child;
	uint32_t last_child;
};

class RCOFileData
{
	uint32_t offset;
	uint8_t *filedata;
	std::string extension;
};

class RCOAttribute
{
public:
	std::string name;
	ATTRIBUTE_TYPE type;
	int i;
	float f;
	std::string s;
	std::u16string c;
	std::vector<uint32_t> ia;
	std::vector<float> fa;
	uint8_t *file;
	uint32_t filelen;
	std::string fileext;
	std::string idstr;
	uint32_t idint;

	std::string toString();
};

class RCOElement
{
public:
	std::string name;
	std::vector<RCOAttribute> attributes;
	std::vector<RCOElement> siblings;
	std::vector<RCOElement> children;
	bool isCompressed = false;
	uint32_t originalSize = 0;
};

enum RCOError
{
	NO_ERROR,
	READ_CHAR_TABLE,
	READ_STRING_TABLE,
	READ_STRING_TABLE_NULL_TERM,
	READ_ID_STR_TABLE_NULL_TERM,
	READ_FILE_DATA,
	READ_STYLE_ID,
	READ_HEADER,
};

class RCO
{
private:
	uint8_t *mBuffer = NULL;
	uint32_t mBufferLen = 0;
	bool mVerbose = true;
	bool mIsRCSF = false;
	int mRCOErrno = NO_ERROR;

	struct rco_header mHeader;
	RCOElement mRootElement;
	std::unordered_map<int, RCOElement> mElements;
	std::unordered_map<std::string, RCOFileData> mFiles;
	std::unordered_map<std::string, RCOElement> mStringTable;
	static std::string fileExtensionFromType(std::string type);
	static void log(std::string str);
	void logf(const char *sformat, ...);
	static std::string typeToString(ATTRIBUTE_TYPE type);

	RCOError getStyleId(std::string &s, uint32_t offset);
	RCOError getIdStringString(std::string &s, uint32_t offset, bool loopback);
	RCOError getIdIntInt(uint32_t &i, uint32_t offset);
	RCOError getCharTableChar(std::u16string &c, uint32_t offset, uint32_t len);
	RCOError getStringTableString(std::string &s, uint32_t offset, uint32_t len);
	RCOError getStringTableString(std::string &s, uint32_t offset);
	RCOError getIntArray(std::vector<uint32_t> &ints, uint32_t offset, uint32_t len);
	RCOError getFloatArray(std::vector<float> &floats, uint32_t offset, uint32_t len);
	RCOError getFileData(uint8_t **filedata, uint32_t &outlen, uint32_t offset, uint32_t size, bool isCompressed, uint32_t originalSize);

	RCOError loadAttributes(RCOElement &el, uint32_t offset, uint32_t count);
	RCOError loadHeader();
	RCOError loadElement(RCOElement &el, uint32_t offset);

protected:
	void dumpElement(FILE *f, RCOElement &el, uint32_t depth, std::string outputDirectory);
public:
	RCO(FILE *f);
	RCO(FILE *f, bool isRCSF);
	RCO(uint8_t *buffer, uint32_t bufferLen);

	RCOElement &getRoot();

	void dump(std::string outputDirectory);

};

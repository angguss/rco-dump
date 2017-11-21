#include <cstdio>
#include <cassert>
#include <zlib.h>

#include "rco.h"

std::string RCO::fileExtensionFromType(std::string type)
{
	if (type == "texture/gim")
		return "gim";
	if (type == "texture/png")
		return "png";
	return "bin";
}

std::string RCOAttribute::toString()
{
	char buf[256];
	memset(buf, 0, 256);
	switch(type)
	{
	case CHAR:
		snprintf(buf, 256, "%c", c);
		break;
	case FLOAT:
		snprintf(buf, 256, "%f", f);
		break;
	case INTEGER_ARRAY:
		{
			int off = 0;
			for (int i = 0; i < ia.size() - 1 && ia.size() > 0; i++)
			{
				snprintf(buf + off, 256 - off, "%d, ", ia[i]);
				off = strlen(buf);
			}
			snprintf(buf + off, 256 - off, "%d", ia[ia.size() - 1]);
		}
		break;
	case FLOAT_ARRAY:
		{
			int off = 0;
			for (int i = 0; i < fa.size() - 1 && fa.size() > 0; i++)
			{
				snprintf(buf + off, 256 - off, "%.3f, ", fa[i]);
				off = strlen(buf);
			}
			snprintf(buf + off, 256 - off, "%.3f", fa[fa.size() - 1]);
		}
		break;
	case ID_STR:
	case ID_STR_LPB:
		snprintf(buf, 256, "%s", idstr.c_str());
		break;
	case STRING:
	case STYLE_ID:
		snprintf(buf, 256, "%s", s.c_str());
		break;
	case INTEGER:
		snprintf(buf, 256, "%u", i);
		break;
	case ID_INT:
		snprintf(buf, 256, "%x", idint);
		break;
	case ID_INT_LPB:
		snprintf(buf, 256, "%x", idint);
		break;
	case DATA:
		snprintf(buf, 256, "%s", s.c_str());
		break;
	}

	return buf;
}

RCOError RCO::getStringTableString(std::string& s, uint32_t offset)
{
	fseek(mF, mHeader.strings_start_off + offset, SEEK_SET);

	char buf[256];
	memset(buf, 0, 256);
	if (fread(&buf, sizeof(char), 255, mF) == 0)
		return READ_STRING_TABLE_NULL_TERM;
	
	s = buf;
	return NO_ERROR;
}

RCOError RCO::getStringTableString(std::string& s, uint32_t offset, uint32_t len)
{
	fseek(mF, mHeader.strings_start_off + offset, SEEK_SET);
	char *buf = new char[len + 1];
	memset(buf, 0, len + 1);
	if (fread(buf, sizeof(char), len, mF) == 0)
		return READ_STRING_TABLE;

	s = buf;
	return NO_ERROR;
}

RCOError RCO::getIdStringString(std::string& s, uint32_t offset, bool loopback = false)
{
	fseek(mF, mHeader.id_str_start_off + offset, SEEK_SET);

	char buf[256];
	uint32_t loopback_val;

	fread(&loopback_val, sizeof(uint32_t), 1, mF);

	printf("Loopback value: %d\n", loopback_val);

	memset(buf, 0, 256);
	if (fread(&buf, sizeof(char), 255, mF) == 0)
		return READ_ID_STR_TABLE_NULL_TERM;

	if (loopback)
	{
		char buf2[256];
		fseek(mF, mHeader.id_str_start_off + loopback_val, SEEK_SET);
		fread(&buf2, sizeof(char), 255, mF);

		printf("done\n");
	}

	s = buf;
	return NO_ERROR;
}

RCOError RCO::getIdIntInt(uint32_t& i, uint32_t offset)
{
	fseek(mF, mHeader.id_int_start_off + offset, SEEK_SET);
	
	uint32_t loopback;

	fread(&loopback, sizeof(uint32_t), 1, mF);
	fread(&i, sizeof(uint32_t), 1, mF);

	return NO_ERROR;
}

RCOError RCO::getFileData(uint8_t **filedata, uint32_t offset, uint32_t size)
{
	fseek(mF, mHeader.file_table_off + offset, SEEK_SET);

	*filedata = new uint8_t[size];

	uint8_t *deflated = new uint8_t[1024 * 1024 * 10];

	uint8_t *fdata = *filedata;

	if (fread(fdata, sizeof(uint8_t), size, mF) == 0)
		return READ_FILE_DATA;

	// We only actually need to do this if 
	uLongf destlen;

	uncompress(deflated, &destlen, fdata, size);

	char buf2[256];

	char *gim_fileext = "gim";
	char *unk_fileext = "bin";
	char *rcsf_fileext = "rcsf";

	if (memcmp(deflated, "MIG", 3) == 0)
	{
		sprintf(buf2, "%d.%s", offset, gim_fileext);
	}
	else if (memcmp(deflated, "RCSF", 4) == 0)
	{
		sprintf(buf2, "%d.%s", offset, rcsf_fileext);
	}
	else
	{
		sprintf(buf2, "%d.%s", offset, unk_fileext);
	}
	
	FILE *dump = fopen(buf2, "wb");
	fwrite(deflated, sizeof(uint8_t), destlen, dump);
	fclose(dump);

	delete[] deflated;

	return NO_ERROR;
	
}

RCOError RCO::getIntArray(std::vector<uint32_t>& ints, uint32_t offset, uint32_t len)
{
	fseek(mF, mHeader.ints_arr_off + (offset * sizeof(uint32_t)), SEEK_SET);

	for (int i = 0; i < len; i++)
	{
		uint32_t val;
		fread(&val, sizeof(uint32_t), 1, mF);
		ints.push_back(val);
	}

	return NO_ERROR;
}

RCOError RCO::getFloatArray(std::vector<float>& floats, uint32_t offset, uint32_t len)
{
	fseek(mF, mHeader.float_arr_off + (offset * sizeof(float)), SEEK_SET);

	for (int i = 0; i < len; i++)
	{
		float val;
		fread(&val, sizeof(float), 1, mF);
		floats.push_back(val);
	}

	return NO_ERROR;
}

RCOError RCO::getStyleId(std::string& s, uint32_t offset)
{
	uint32_t styleid;

	fseek(mF, mHeader.styles_off + offset, SEEK_SET);

	if (fread(&styleid, sizeof(uint32_t), 1, mF) == 0)
		return READ_STYLE_ID;
	
	char buf[256];
	snprintf(buf, 256, "%x", styleid);

	s = buf;

	return NO_ERROR;
}


RCOError RCO::loadAttributes(RCOElement &el, uint32_t offset, uint32_t count)
{
	std::vector<RCOAttribute> &attributes = el.attributes;

	rco_tree_table_element_attribute_raw *raw_attr = new rco_tree_table_element_attribute_raw[count];

	fseek(mF, offset, SEEK_SET);
	fread(raw_attr, sizeof(rco_tree_table_element_attribute_raw), count, mF);
	

	for (int i = 0; i < count; i++)
	{
		RCOAttribute attr;
		
		attr.type = static_cast<ATTRIBUTE_TYPE>(raw_attr[i].type);
		getStringTableString(attr.name, raw_attr[i].string_offset);

		if (attr.name == "style")
			printf("yes");

		switch(attr.type)
		{
		case STRING:
			getStringTableString(attr.s, raw_attr[i].s.offset, raw_attr[i].s.len);
			break;
		case ID_STR_LPB:
			printf("ID_STR_LPB\n");
			getIdStringString(attr.idstr, raw_attr[i].id.offset, true);
			break;
		case ID_STR:
			printf("ID_STR\r\n");
			getIdStringString(attr.idstr, raw_attr[i].id.offset);
			break;
		case STYLE_ID:
			getStyleId(attr.s, raw_attr[i].s.offset);
			break;
		case FLOAT:
			attr.f = raw_attr[i].f;
			break;
		case INTEGER:
			attr.i = raw_attr[i].i;
			break;
		case INTEGER_ARRAY:
			getIntArray(attr.ia, raw_attr[i].ia.offset, raw_attr[i].ia.num);
			break;
		case FLOAT_ARRAY:
			getFloatArray(attr.fa, raw_attr[i].fa.offset, raw_attr[i].fa.num);
			break;
		case ID_INT:
		case ID_INT_LPB:
			getIdIntInt(attr.idint, raw_attr[i].idref.offset);
			break;
		case DATA:
			printf("Filedata\n");
			getFileData(&attr.file, raw_attr[i].file.offset, raw_attr[i].file.size);
			break;
		default:
			printf("unhandled type: %d\n", attr.type);
		}

		attributes.push_back(attr);
	}

	std::string path;
	std::string ext;
	std::string id;

	if (el.name == "locale")
	{
		path = "/xmls";
		ext = "xml";
	}
	else if (el.name == "texture")
	{
		path = "/textures";
		ext = "gim";
	}

	for (auto it = attributes.begin(); it != attributes.end(); it++)
	{
		if ((*it).name == "id")
			id = (*it).toString();
		if ((*it).name == "type")
			ext = RCO::fileExtensionFromType((*it).toString());
	}

	for (auto it = attributes.begin(); it != attributes.end(); it++)
	{
		if ((*it).name == "src")
		{
			char buf[256];
			sprintf(buf, "%s/%s.%s", path.c_str(), id.c_str(), ext.c_str());
			(*it).s = buf;
		}
	}

	return NO_ERROR;
}


RCOError RCO::loadHeader()
{
	fseek(mF, 0, SEEK_SET);
	if (fread(&mHeader, sizeof(rco_header), 1, mF) == 0)
		return READ_HEADER;

	return NO_ERROR;
}

void RCO::registerElement(RCOElement& el, uint32_t absolute_offset)
{
	if (mElements.count(absolute_offset) <= 0)
		mElements.emplace(absolute_offset, el);
}

bool RCO::tryGetElement(uint32_t absolute_offset, RCOElement &el)
{
	if (mElements.count(absolute_offset) > 0)
	{
		el = mElements.at(absolute_offset);
		return true;
	}
	return false;
}


RCOError RCO::loadElement(RCOElement &el, uint32_t offset)
{
	RCOError err;

	rco_tree_table_element element;

	fseek(mF, offset, SEEK_SET);
	fread(&element, sizeof(rco_tree_table_element), 1, mF);

	uint32_t root_attr_offset = ftell(mF);

	err = getStringTableString(el.name, element.element_offset);

	if (err != NO_ERROR)
		return err;

	err = loadAttributes(el, root_attr_offset, element.attr_ct);

	if (err != NO_ERROR)
		return err;

	if (element.first_child != 0xFFFFFFFF)
	{
		RCOElement first_child;
		err = loadElement(first_child, mHeader.tree_start_off + element.first_child);
		if (err != NO_ERROR)
			return err;
		el.children.push_back(first_child);
	}

	if (element.next_sibling != 0xFFFFFFFF)
	{
		RCOElement next_sibling;
		err = loadElement(next_sibling, mHeader.tree_start_off + element.next_sibling);
		if (err != NO_ERROR)
			return err;
		el.siblings.push_back(next_sibling);
	}

	return err;
}

RCO::RCO(FILE* f) : mF(f)
{
	if (mF == nullptr)
		return;
	mRCOErrno = loadHeader();
	if (mRCOErrno != NO_ERROR)
		return;

	mRCOErrno = loadElement(mRootElement, mHeader.tree_start_off);

	if (mRCOErrno != NO_ERROR)
		printf("Error\n");
}

void RCO::dumpElement(FILE *f, RCOElement &el)
{
	fprintf(f, "<%s", el.name.c_str());
	for (auto it = el.attributes.begin(); it != el.attributes.end(); it++)
		fprintf(f, " %s=\"%s\"", (*it).name.c_str(), (*it).toString().c_str());

	if (el.children.size() > 0)
	{
		fprintf(f, ">\r\n");
		dumpElement(f, el.children.front());
		fprintf(f, "</%s>\r\n", el.name.c_str());
	}
	else
	{
		fprintf(f, " />\r\n");
	}

	if (el.siblings.size() > 0)
		dumpElement(f, el.siblings.front());	
}

void RCO::dump(FILE *f)
{
	if (!f)
		return;

	dumpElement(f, mRootElement);
}

RCOElement &RCO::getRoot()
{
	return mRootElement;
}

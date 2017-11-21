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
	if (type == "sound/vag")
		return "vag";
	return "bin";
}

std::string RCOAttribute::toString()
{
	char buf[256 + 1];
	memset(buf, 0, 256 + 1);
	switch (type)
	{
		/*case CHAR:
		for (int i = 0, j = 0; i < c.size(); i++)
		{
		char ca = c.c_str()[i];
		if (ca != '\0')
		{
		snprintf(buf + j, 256 - j, "%c", ca);
		j++;
		}

		}

		break;*/
	case FLOAT:
		snprintf(buf, 256, "%.2f", f);
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
			snprintf(buf + off, 256 - off, "%.2f, ", fa[i]);
			off = strlen(buf);
		}
		snprintf(buf + off, 256 - off, "%.2f", fa[fa.size() - 1]);
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

RCOError RCO::getCharTableChar(std::string &c, uint32_t offset, uint32_t len)
{
	fseek(mF, mHeader.char_start_off + (offset * 2), SEEK_SET);

	char *buf = new char[len + 1];
	memset(buf, 0, len + 1);

	if (fread(buf, sizeof(char), len, mF) == 0)
		return READ_CHAR_TABLE;

	c = std::string(buf, len);

	delete[] buf;

	return NO_ERROR;
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

RCOError RCO::getFileData(uint8_t **filedata, uint32_t &outlen, uint32_t offset, uint32_t size, bool isCompressed)
{
	fseek(mF, mHeader.file_table_off + offset, SEEK_SET);

	*filedata = new uint8_t[size];
	
	uint8_t *fdata = *filedata;
	outlen = size;

	if (fread(fdata, sizeof(uint8_t), size, mF) == 0)
		return READ_FILE_DATA;

	if (isCompressed)
	{
		uint8_t *deflated = new uint8_t[1024 * 1024 * 10];

		uLongf destlen;

		uncompress(deflated, &destlen, fdata, size);

		char buf2[256];

		char *gim_fileext = "gim";
		char *unk_fileext = "bin";
		char *rcsf_fileext = "rcsf";
		char *dds_fileext = "dds";

		if (memcmp(deflated, "MIG", 3) == 0)
		{
			sprintf(buf2, "%d.%s", offset, gim_fileext);
		}
		else if (memcmp(deflated, "DDS", 3) == 0)
		{
			sprintf(buf2, "%d.%s", offset, dds_fileext);
		}
		else if (memcmp(deflated, "RCSF", 4) == 0)
		{
			sprintf(buf2, "%d.%s", offset, rcsf_fileext);
		}
		else
		{
			sprintf(buf2, "%d.%s", offset, unk_fileext);
		}

		uint8_t *deflated_final = new uint8_t[destlen];
		memcpy(deflated_final, deflated, destlen);

		*filedata = deflated_final;
		outlen = destlen;

		delete[] deflated;
		delete[] fdata;
	}

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

	std::unordered_map<int, RCOAttribute> filedata_indexes;
	
	// Handle all attributes apart from files
	for (int i = 0; i < count; i++)
	{
		RCOAttribute attr;

		attr.type = static_cast<ATTRIBUTE_TYPE>(raw_attr[i].type);
		getStringTableString(attr.name, raw_attr[i].string_offset);
		
		bool deferred = false;

		switch (attr.type)
		{
		case CHAR:
			getCharTableChar(attr.c, raw_attr[i].c.offset, raw_attr[i].c.len);
			break;
		case STRING:
			getStringTableString(attr.s, raw_attr[i].s.offset, raw_attr[i].s.len);
			break;
		case ID_STR_LPB:
			getIdStringString(attr.idstr, raw_attr[i].id.offset, true);
			break;
		case ID_STR:
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
			filedata_indexes.emplace(i, attr);
			deferred = true;
			break;
		default:
			printf("unhandled type: %d\n", attr.type);
		}

		if (attr.name == "compress")
			el.isCompressed = true;
		if (!deferred)
			attributes.push_back(attr);
	}

	// Handle files afterwards (deferred)
	for (auto it = filedata_indexes.begin(), end = filedata_indexes.end(); it != end; it++)
	{
		int index = (*it).first;
		RCOAttribute &attr = (*it).second;

		if (attr.type == DATA)
		{
			getFileData(&attr.file, attr.filelen, raw_attr[index].file.offset, raw_attr[index].file.size, el.isCompressed);
		}

		attributes.push_back(attr);
	}


	std::string path;
	std::string ext;
	std::string id;

	if (el.name == "locale")
	{
		path = "xmls";
		ext = "xml";
	}
	else if (el.name == "texture")
	{
		path = "textures";
	}
	else if (el.name == "sounddata")
	{
		path = "sounds";
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
		else if ((*it).name == "right")
		{
			char buf[256];
			sprintf(buf, "%s/%s_right.%s", path.c_str(), id.c_str(), ext.c_str());
			(*it).s = buf;
		}
		else if ((*it).name == "left")
		{
			char buf[256];
			sprintf(buf, "%s/%s_left.%s", path.c_str(), id.c_str(), ext.c_str());
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

RCO::RCO(FILE* f) : RCO(f, false)
{
}

RCO::RCO(FILE *f, bool isRCSF) : mF(f), mIsRCSF(isRCSF)
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

void RCO::dumpElement(FILE *f, RCOElement &el, uint32_t depth = 0)
{
	fprintf(f, "%*c<%s", depth, ' ', el.name.c_str());

	for (auto it = el.attributes.begin(); it != el.attributes.end(); it++)
	{
		RCOAttribute &attr = (*it);
		fprintf(f, " %s=\"%s\"", attr.name.c_str(), attr.toString().c_str());
		if (
			(attr.name == "src" || attr.name == "right" || attr.name == "left") && 
			!mIsRCSF)
		{
			FILE *outfile = fopen(attr.toString().c_str(), "wb");
			fwrite(attr.file, sizeof(uint8_t), attr.filelen, outfile);
			fclose(outfile);

			// XML files are actually rcsf
			//if (attr.toString().find(".xml") != std::string::npos)
			//{
			//	FILE *infil = fopen(attr.toString().c_str(), "rb");
			//	RCO tmprco(infil, true);
			//	fclose(infil);


			//	outfile = fopen(attr.toString().c_str(), "wb");
			//	tmprco.dump(outfile);
			//	fclose(outfile);
			//}
		}
	}


	if (el.children.size() > 0)
	{
		fprintf(f, ">\r\n");
		dumpElement(f, el.children.front(), depth + 1);
		fprintf(f, "%*c</%s>\r\n", depth, ' ', el.name.c_str());
	}
	else
	{
		fprintf(f, " />\r\n");
	}

	if (el.siblings.size() > 0)
		dumpElement(f, el.siblings.front(), depth);
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

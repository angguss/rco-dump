#include <cstdio>
#include <cassert>
#include <zlib.h>
#include <locale>
#include <codecvt>

#ifdef __GNUC__
#include <cstring>
#endif

#include <platform.h>
#include <rco.h>
#include "layout.h"

std::string RCO::fileExtensionFromType(std::string type)
{
	if (type == "texture/jpg")
		return "jpg";
	if (type == "texture/tif")
		return "tif";
	if (type == "texture/gif")
		return "gif";
	if (type == "texture/gxt")
		return "gxt";
	if (type == "texture/gim")
		return "gim";
	if (type == "texture/png")
		return "png";
	// Some PS4 RCOs have just png and not texture/png
	if (type == "png")
		return "png";
	if (type == "texture/dds")
		return "dds";
	if (type == "sound/vag")
		return "vag";
	if (type == "sound/wav")
		return "wav";
	if (type == "sound/mp3")
		return "mp3";
	if (type == "sound/at3")
		return "at3";
	if (type == "sound/aac")
		return "aac";
	return "bin";
}

std::string RCO::typeToString(ATTRIBUTE_TYPE type)
{
	switch (type)
	{
	case CHAR:
		return "CHAR";
	case DATA:
		return "DATA";
	case FLOAT:
		return "FLOAT";
	case FLOAT_ARRAY:
		return "FLOAT_ARRAY";
	case ID_INT: 
		return "ID_INT";
	case ID_INT_LPB:
		return "ID_INT_LPB";
	case ID_STR_LPB:
		return "ID_STR_LPB";
	case ID_STR:
		return "ID_STR";
	case INTEGER:
		return "INTEGER";
	case INTEGER_ARRAY:
		return "INTEGER_ARRAY";
	case NONE:
		return "NONE";
	case STRING:
		return "STRING";
	case STYLE_ID:
		return "STYLE_ID";
	}

	return "NO KNOWN TYPE";
}

void RCO::log(std::string str)
{
	printf("%s\n", str.c_str());
}

void RCO::logf(const char* sformat, ...)
{
	if (!mVerbose)
		return;

	char buf[1000];
	memset(buf, 0, (999 + 1) * sizeof(char));
	va_list marker;
	va_start(marker, sformat);
	vsnprintf(buf, 999, sformat, marker);
	va_end(marker);
	printf("%s\n", buf);
}

std::string RCOAttribute::toString()
{
	char buf[256 + 1];
	memset(buf, 0, 256 + 1);
	switch (type)
	{
	case CHAR:
		return utf16_to_utf8(c);
		break;
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
	default:
		printf("UNSUPPORTED TYPE\n");
	}

	return buf;
}

RCOError RCO::getCharTableChar(std::u16string &c, uint32_t offset, uint32_t len)
{
	char16_t *buf = new char16_t[len + 2];
	memset(buf, 0, len + 2);

	memcpy(buf, mBuffer + mHeader.char_start_off + (offset * 2), len);

	c = std::u16string(buf, len);

	delete[] buf;

	return NO_ERROR;
}

RCOError RCO::getStringTableString(std::string& s, uint32_t offset)
{
	char buf[256];
	memset(buf, 0, 256);

	memcpy(&buf, mBuffer + mHeader.strings_start_off + offset, 255 * sizeof(char));

	s = buf;
	return NO_ERROR;
}

RCOError RCO::getStringTableString(std::string& s, uint32_t offset, uint32_t len)
{
	char *buf = new char[len + 1];
	memset(buf, 0, len + 1);
	memcpy(buf, mBuffer + mHeader.strings_start_off + offset, sizeof(char) * len);

	s = buf;
	return NO_ERROR;
}

RCOError RCO::getIdStringString(std::string& s, uint32_t offset, bool loopback = false)
{
	char buf[256];
	uint32_t loopback_val;

	memcpy(&loopback_val, mBuffer + mHeader.id_str_start_off + offset, sizeof(uint32_t));
	memset(buf, 0, 256);
	memcpy(buf, mBuffer + mHeader.id_str_start_off + offset + sizeof(uint32_t), 255 * sizeof(char));

	s = buf;
	return NO_ERROR;
}

RCOError RCO::getIdIntInt(uint32_t& i, uint32_t offset)
{
	uint32_t loopback;

	memcpy(&loopback, mBuffer + mHeader.id_int_start_off + offset, sizeof(uint32_t));
	memcpy(&i, mBuffer + mHeader.id_int_start_off + offset + sizeof(uint32_t), sizeof(uint32_t));

	return NO_ERROR;
}

RCOError RCO::getFileData(uint8_t **filedata, uint32_t &outlen, uint32_t offset, uint32_t size, bool isCompressed, uint32_t originalSize)
{
	*filedata = new uint8_t[size];

	uint8_t *fdata = *filedata;
	outlen = size;

	memcpy(fdata, mBuffer + mHeader.file_table_off + offset, size * sizeof(uint8_t));

	if (isCompressed)
	{
		if (originalSize <= 0)
			return NO_ERROR;

		uint8_t *deflated = new uint8_t[originalSize];

		uLongf destlen = originalSize;//1024 * 1024 * 10;

		uncompress(deflated, &destlen, fdata, size);

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
	for (int i = 0; i < len; i++)
	{
		uint32_t val;
		memcpy(&val, mBuffer + mHeader.ints_arr_off + (offset * sizeof(uint32_t)) + i * sizeof(uint32_t), sizeof(uint32_t));
		ints.push_back(val);
	}

	return NO_ERROR;
}

RCOError RCO::getFloatArray(std::vector<float>& floats, uint32_t offset, uint32_t len)
{
	for (int i = 0; i < len; i++)
	{
		float val;
		memcpy(&val, mBuffer + mHeader.float_arr_off + (offset * sizeof(float)) + i * sizeof(float), sizeof(float));
		floats.push_back(val);
	}

	return NO_ERROR;
}

RCOError RCO::getStyleId(std::string& s, uint32_t offset)
{
	uint32_t styleid;

	memcpy(&styleid, mBuffer + mHeader.styles_off + offset * sizeof(uint32_t), sizeof(uint32_t));

	char buf[256];
	snprintf(buf, 256, "%x", styleid);

	s = buf;

	return NO_ERROR;
}

RCOError RCO::loadAttributes(RCOElement &el, uint32_t offset, uint32_t count)
{
	std::vector<RCOAttribute> &attributes = el.attributes;

	rco_tree_table_element_attribute_raw *raw_attr = new rco_tree_table_element_attribute_raw[count];

	memcpy(raw_attr, mBuffer + offset, sizeof(rco_tree_table_element_attribute_raw) * count);

	std::unordered_map<int, RCOAttribute> filedata_indexes;

	// Handle all attributes apart from files
	for (int i = 0; i < count; i++)
	{
		RCOAttribute attr;

		rco_tree_table_element_attribute_raw &cur_attr = raw_attr[i];

		attr.type = static_cast<ATTRIBUTE_TYPE>(cur_attr.type);
		getStringTableString(attr.name, cur_attr.string_offset);

		bool deferred = false;

		switch (attr.type)
		{
		case CHAR:
			getCharTableChar(attr.c, cur_attr.c.offset, cur_attr.c.len);
			break;
		case STRING:
			getStringTableString(attr.s, cur_attr.s.offset, cur_attr.s.len);
			break;
		case ID_STR_LPB:
			getIdStringString(attr.idstr, cur_attr.id.offset, true);
			break;
		case ID_STR:
			getIdStringString(attr.idstr, cur_attr.id.offset);
			break;
		case STYLE_ID:
			getStyleId(attr.s, cur_attr.s.offset);
			break;
		case FLOAT:
			attr.f = cur_attr.f;
			break;
		case INTEGER:
			attr.i = cur_attr.i;
			break;
		case INTEGER_ARRAY:
			getIntArray(attr.ia, cur_attr.ia.offset, cur_attr.ia.num);
			break;
		case FLOAT_ARRAY:
			getFloatArray(attr.fa, cur_attr.fa.offset, cur_attr.fa.num);
			break;
		case ID_INT:
		case ID_INT_LPB:
			getIdIntInt(attr.idint, cur_attr.idref.offset);
			break;
		case DATA:
			filedata_indexes.emplace(i, attr);
			deferred = true;
			break;
		default:
			printf("unhandled type: %d\n", attr.type);
		}

		if (attr.name == "compress" && attr.toString() == "on")
			el.isCompressed = true;
		else if (attr.name == "origsize")
			el.originalSize = attr.i;

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
			getFileData(&attr.file, attr.filelen, raw_attr[index].file.offset, raw_attr[index].file.size, el.isCompressed, el.originalSize);
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
	else if (el.name == "texture" || el.name == "override_texture")
	{
		path = "textures";
	}
	else if (el.name == "sounddata")
	{
		path = "sounds";
	}
	else if (el.name == "layout")
	{
		path = "layouts";
		// Just a binary file but makes sense to give it a separate ext
		ext = "pslayout";
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
		else if ((*it).name == "src_4k")
		{
			char buf[256];
			sprintf(buf, "%s/%s_4k.%s", path.c_str(), id.c_str(), ext.c_str());
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

	delete[] raw_attr;

	return NO_ERROR;
}

RCOError RCO::loadHeader()
{
	memcpy(&mHeader, mBuffer, sizeof(rco_header));

	return NO_ERROR;
}

RCOError RCO::loadElement(RCOElement &el, uint32_t offset)
{
	RCOError err;

	rco_tree_table_element element;

	memcpy(&element, mBuffer + offset, sizeof(rco_tree_table_element));

	uint32_t root_attr_offset = offset + sizeof(rco_tree_table_element);

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

void RCO::dumpElement(FILE *f, RCOElement &el, uint32_t depth = 0, std::string outputDirectory = "")
{
	writeIndent(f, depth);
	fprintf(f, "<%s", el.name.c_str());

	for (auto it = el.attributes.begin(); it != el.attributes.end(); it++)
	{
		RCOAttribute &attr = (*it);
		fprintf(f, " %s=\"%s\"", attr.name.c_str(), attr.toString().c_str());

		if (attr.type == DATA)
		{
			// Handle locale XML files
			if (attr.toString().find(".xml") != std::string::npos)
			{
				uint8_t *buffercopy = new uint8_t[attr.filelen];
				memcpy(buffercopy, attr.file, attr.filelen);
				RCO tmprco(buffercopy, attr.filelen);

				FILE *outfile = fopen((outputDirectory + "/" + attr.toString()).c_str(), "wb");

				tmprco.dumpElement(outfile, tmprco.getRoot(), 0, outputDirectory);

				fclose(outfile);

				delete[] buffercopy;
			}
			else if (attr.toString().find(".pslayout") != std::string::npos)
			{
				uint8_t *buffercopy = new uint8_t[attr.filelen];
				memcpy(buffercopy, attr.file, attr.filelen);
				LayoutFile layout(buffercopy);
				
				
				std::string output_filename = outputDirectory + "/" + attr.toString() + ".xml";
				FILE *outfile = fopen(output_filename.c_str(), "wb");
				
				if (outfile != nullptr)
				{
					layout.write(outfile);
					fclose(outfile);
				}
				

			}
			// Handle anything else (textures, sounds, etc)
			else
			{
				std::string output_filename = outputDirectory + "/" + attr.toString();
				FILE *outfile = fopen(output_filename.c_str(), "wb");
				if (outfile != nullptr)
				{
					fwrite(attr.file, sizeof(uint8_t), attr.filelen, outfile);
					fclose(outfile);
				}
				else
				{
					printf("ERROR: Couldn't open file for writing: %s\n", output_filename.c_str());
				}
			}
		}
	}

	if (el.children.size() > 0)
	{
		fprintf(f, ">\r\n");
		dumpElement(f, el.children.front(), depth + 1, outputDirectory);
		writeIndent(f, depth);
		fprintf(f, "</%s>\r\n", el.name.c_str());
	}
	else
	{
		fprintf(f, " />\r\n");
	}

	if (el.siblings.size() > 0)
		dumpElement(f, el.siblings.front(), depth, outputDirectory);
}

void RCO::dump(std::string outputDirectory)
{
	createDirectory(outputDirectory + "/");
	createDirectory(outputDirectory + "/xmls/");
	createDirectory(outputDirectory + "/textures/");
	createDirectory(outputDirectory + "/sounds/");
	createDirectory(outputDirectory + "/layouts/");

	std::string outputFile = outputDirectory + "/index.xml";

	FILE *f = fopen(outputFile.c_str(), "wb");

	fprintf(f, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n");
	dumpElement(f, mRootElement, 0, outputDirectory);

	fclose(f);
}

RCOElement &RCO::getRoot()
{
	return mRootElement;
}


RCO::RCO(FILE* f) : RCO(f, false)
{
}

RCO::RCO(FILE *f, bool isRCSF) : mIsRCSF(isRCSF)
{
	if (f == nullptr)
		return;

	fseek(f, 0, SEEK_END);
	mBufferLen = ftell(f);
	fseek(f, 0, SEEK_SET);

	mBuffer = new uint8_t[mBufferLen];
	fread(mBuffer, sizeof(uint8_t), mBufferLen, f);

	mRCOErrno = loadHeader();
	if (mRCOErrno != NO_ERROR)
		return;

	mRCOErrno = loadElement(mRootElement, mHeader.tree_start_off);

	if (mRCOErrno != NO_ERROR)
		printf("Error\n");
}

RCO::RCO(uint8_t *buffer, uint32_t bufferLen) : mBuffer(buffer), mBufferLen(bufferLen)
{
	mRCOErrno = loadHeader();
	if (mRCOErrno != NO_ERROR)
		return;

	mRCOErrno = loadElement(mRootElement, mHeader.tree_start_off);

	if (mRCOErrno != NO_ERROR)
		printf("Error!\n");
}

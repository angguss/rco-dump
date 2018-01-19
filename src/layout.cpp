#include <sstream> 
#include "util.h"
#include "layout.h"

int Transform::read(uint8_t* buf, Transform &out)
{
	buf += Util::readFloat32(buf, out.transX);
	buf += Util::readFloat32(buf, out.transY);
	buf += Util::readFloat32(buf, out.transZ);

	buf += Util::readFloat32(buf, out.scaleX);
	buf += Util::readFloat32(buf, out.scaleY);
	buf += Util::readFloat32(buf, out.scaleZ);

	buf += Util::readFloat32(buf, out.angleX);
	buf += Util::readFloat32(buf, out.angleY);
	buf += Util::readFloat32(buf, out.angleZ);

	return sizeof(out.transX) * 9;
}

std::string Transform::writeXmlAttribute() const
{
	std::stringstream str;
	
	str << transX << ",";
	str << transY << ",";
	str << transZ << ",";
	
	str << scaleX << ",";
	str << scaleY << ",";
	str << scaleZ << ",";
	
	str << angleX << ",";
	str << angleY << ",";
	str << angleZ;

	return str.str();
}

int Color::read(uint8_t *buf, Color &out)
{
	buf += Util::readFloat32(buf, out.r);
	buf += Util::readFloat32(buf, out.g);
	buf += Util::readFloat32(buf, out.b);
	buf += Util::readFloat32(buf, out.a);

	return sizeof(out.r) * 4;
}

std::string Color::writeXml(int indent, std::string name) const
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<" << name << ">" << std::endl;

	Util::writeXmlElement(str, indent + 1, "R", r);
	Util::writeXmlElement(str, indent + 1, "G", g);
	Util::writeXmlElement(str, indent + 1, "B", b);
	Util::writeXmlElement(str, indent + 1, "A", a);

	str << Util::getIndent(indent) << "</" << name << ">" << std::endl;
	return str.str();
}

int TextShadow::read(uint8_t* buf, TextShadow &out)
{
	int colorSize = Color::read(buf, out.color);
	buf += colorSize;
	buf += Util::readFloat32(buf, out.xOffset);
	buf += Util::readFloat32(buf, out.yOffset);

	return colorSize + (2 * sizeof(out.xOffset));
}

std::string TextShadow::writeXml(int indent) const
{
	std::stringstream str;

	str << Util::getIndent(indent) << "<TextShadow>" << std::endl;
	str << color.writeXml(indent + 1);
	Util::writeXmlElement(str, indent + 1, "XOffset", xOffset);
	Util::writeXmlElement(str, indent + 1, "YOffset", yOffset);
	str << Util::getIndent(indent) << "</TextShadow>" << std::endl;

	return str.str();
}

int NinePatchMargin::read(uint8_t* buf, NinePatchMargin &out)
{
	buf += Util::readInt32(buf, out.top);
	buf += Util::readInt32(buf, out.bottom);
	buf += Util::readInt32(buf, out.left);
	buf += Util::readInt32(buf, out.right);

	return sizeof(out.top) * 4;
}

std::string NinePatchMargin::writeXml(int indent) const
{
	std::stringstream str;
	
	str << Util::getIndent(indent) << "<NinePatchMargin>" << std::endl;
	Util::writeXmlElement(str, indent + 1, "Top", top);
	Util::writeXmlElement(str, indent + 1, "Left", left);
	Util::writeXmlElement(str, indent + 1, "Right", right);
	Util::writeXmlElement(str, indent + 1, "Bottom", bottom);
	str << Util::getIndent(indent) << "</NinePatchMargin>" << std::endl;

	return str.str();
}

int FontData::read(uint8_t* buf, FontData &out)
{
	buf += Util::readString(buf, out.fontFamily);
	buf += Util::readInt32(buf, out.fontSize);
	buf += Util::readInt32(buf, out.fontStyle);
	buf += Util::readInt32(buf, out.fontWeight);

	return (sizeof(out.fontSize) * 3) + out.fontFamily.length() + sizeof(uint8_t);
}

std::string FontData::writeXml(int indent, std::string name) const
{
	std::stringstream str;

	str << Util::getIndent(indent) << "<" << name << ">" << std::endl;
	Util::writeXmlElement(str, indent + 1, "FontFamily", fontFamily);
	Util::writeXmlElement(str, indent + 1, "FontSize", fontSize);
	Util::writeXmlElement(str, indent + 1, "FontStyle", fontStyle);
	Util::writeXmlElement(str, indent + 1, "FontWeight", fontWeight);
	str << Util::getIndent(indent) << "</" << name << ">" << std::endl;

	return str.str();
}


uint8_t *Widget::read(uint8_t *buf)
{
	buf += Util::readInt32(buf, type);
	buf += Util::readInt32(buf, numChildren);
	buf += Util::readString(buf, stylefile);
	buf += Util::readString(buf, name);
	buf += Util::readFloat32(buf, x);
	buf += Util::readFloat32(buf, y);
	buf += Util::readFloat32(buf, width);
	buf += Util::readFloat32(buf, height);
	buf += Util::readBool(buf, visible);
	
	int tmp;
	
	buf += Util::readInt32(buf, tmp);
	anchor = static_cast<Anchor>(tmp);
	buf += Transform::read(buf, transform);

	return buf;
}

void Widget::writeXmlAttributes(std::stringstream &str) const
{
	Util::writeXmlAttribute(str, "type", type);
	Util::writeXmlAttribute(str, "styleFile", stylefile);
	Util::writeXmlAttribute(str, "name", name);
	Util::writeXmlAttribute(str, "x", x);
	Util::writeXmlAttribute(str, "y", y);
	Util::writeXmlAttribute(str, "width", width);
	Util::writeXmlAttribute(str, "height", height);
	Util::writeXmlAttribute(str, "visible", visible);
	Util::writeXmlAttribute(str, "anchor", Util::toString(anchor));
	Util::writeXmlAttribute(str, "transform", transform.writeXmlAttribute());
}

void Widget::writeXmlElements(std::stringstream &str, int indent) const
{
	Util::writeXmlElement(str, indent, "Type", type);
	Util::writeXmlElement(str, indent, "StyleFile", stylefile);
	Util::writeXmlElement(str, indent, "Name", name);
	Util::writeXmlElement(str, indent, "X", x);
	Util::writeXmlElement(str, indent, "Y", y);
	Util::writeXmlElement(str, indent, "Width", width);
	Util::writeXmlElement(str, indent, "Height", height);
	Util::writeXmlElement(str, indent, "Visible", visible);
	Util::writeXmlElement(str, indent, "Anchor", Util::toString(anchor));
}

void Widget::writeXmlChildren(std::stringstream& str, int indent) const
{
	str << Util::getIndent(indent) << "<Widgets>" << std::endl;
	for (int i = 0; i < children.size(); i++)
	{
		str << children[i]->writeXml(indent + 1) << std::endl;
	}
	str << Util::getIndent(indent) << "</Widgets>" << std::endl;
}


uint8_t* BusyIndicator::read(uint8_t* buf)
{
	buf += Util::readBool(buf, animating);
	buf = Widget::read(buf);
	return buf;
}


uint8_t* ProgressBar::read(uint8_t* buf)
{
	int tmp;
	buf += Util::readInt32(buf, tmp);
	progressBarType = static_cast<ProgressBarType>(tmp);
	buf += Util::readFloat32(buf, progress);
	return Widget::read(buf);
}

std::string ProgressBar::writeXml(int indent)
{
	std::stringstream str;
	
	str << Util::getIndent(indent) << "<ProgressBar";
	writeXmlAttributes(str);
	Util::writeXmlAttribute(str, "ProgressBarType", progressBarType);
	Util::writeXmlAttribute(str, "Progress", progress);
	str << "/>" << std::endl;

	return str.str();
}

uint8_t* Button::read(uint8_t* buf)
{
	buf += Util::readBool(buf, isTextButton);
	buf += Util::readBool(buf, isCustomBackground);
	buf += Util::readString(buf, text);
	
	buf += Color::read(buf, textColor);
	buf += FontData::read(buf, textFont);
	buf += Util::readInt32(buf, textTrimming);
	buf += Util::readBool(buf, hasTextShadow);
	buf += TextShadow::read(buf, textShadow);
	buf += NinePatchMargin::read(buf, margin);
	
	buf += Util::readString(buf, backgroundNormalImage);
	buf += Util::readString(buf, backgroundPressedImage);
	buf += Util::readString(buf, backgroundDisabledImage);

	buf += Util::readString(buf, iconImage);

	buf += Color::read(buf, backgroundFilter);
	buf = Widget::read(buf);

	return buf;
}

std::string Button::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<Button";
	Util::writeXmlAttribute(str, "isTextButton", isTextButton);
	Util::writeXmlAttribute(str, "text", text);
	Util::writeXmlAttribute(str, "backgroundNormalImage", backgroundNormalImage);
	Util::writeXmlAttribute(str, "backgroundPressedImage", backgroundPressedImage);
	Util::writeXmlAttribute(str, "backgroundDisabledImage", backgroundDisabledImage);
	writeXmlAttributes(str);

	if (numChildren == 0)
		str << "/>";
	else
	{
		str << ">" << std::endl;

		Widget::writeXmlChildren(str, indent + 1);

		str << Util::getIndent(indent) << "</Button>";
	}

	return str.str();
}

uint8_t* GridListPanel::read(uint8_t* buf)
{
	int tmp;
	buf += Util::readInt32(buf, tmp);
	scrollbarVisibility = static_cast<ScrollBarVisibility>(tmp);
	buf += Util::readInt32(buf, tmp);
	scrollOrientation = static_cast<GridListScrollOrientation>(tmp);

	buf += Util::readFloat32(buf, itemWidth);
	buf += Util::readFloat32(buf, itemHeight);
	buf += Util::readString(buf, listPanelItemName);
	return Widget::read(buf);
}

std::string GridListPanel::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<GridListPanel";
		writeXmlAttributes(str);
	str << ">" << std::endl;

	Util::writeXmlElement(str, indent + 1, "ScrollBarVisibility", scrollbarVisibility);
	Util::writeXmlElement(str, indent + 1, "ScrollOrientation", scrollOrientation);
	Util::writeXmlElement(str, indent + 1, "ItemWidth", itemWidth);
	Util::writeXmlElement(str, indent + 1, "ItemHeight", itemHeight);
	Util::writeXmlElement(str, indent + 1, "ListPanelItemName", listPanelItemName);

	Widget::writeXmlChildren(str, indent + 1);

	str << Util::getIndent(indent) << "</GridListPanel>";
	return str.str();
}

uint8_t* ScrollPanel::read(uint8_t* buf)
{
	buf += Util::readFloat32(buf, panelX);
	buf += Util::readFloat32(buf, panelY);
	buf += Util::readFloat32(buf, panelWidth);
	buf += Util::readFloat32(buf, panelHeight);
	buf += Util::readBool(buf, horizontalScroll);
	buf += Util::readBool(buf, verticalScroll);
	int tmp;
	buf += Util::readInt32(buf, tmp);
	scrollBarVisibility = static_cast<ScrollBarVisibility>(tmp);
	buf += Util::readString(buf, panelName);
	return Widget::read(buf);
}

std::string ScrollPanel::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<ScrollPanel";
	writeXmlAttributes(str);
	str << ">" << std::endl;

	Util::writeXmlElement(str, indent + 1, "PanelX", panelX);
	Util::writeXmlElement(str, indent + 1, "PanelY", panelY);
	Util::writeXmlElement(str, indent + 1, "PanelWidth", panelWidth);
	Util::writeXmlElement(str, indent + 1, "PanelHeight", panelHeight);
	Util::writeXmlElement(str, indent + 1, "PanelName", panelName);
	Util::writeXmlElement(str, indent + 1, "HorizontalScroll", horizontalScroll);
	Util::writeXmlElement(str, indent + 1, "VerticalScroll", verticalScroll);
	Util::writeXmlElement(str, indent + 1, "ScrollBarVisibility", scrollBarVisibility);

	writeXmlChildren(str, indent + 1);

	str << Util::getIndent(indent) << "</ScrollPanel>";
	return str.str();
}

uint8_t* ListPanelItem::read(uint8_t* buf)
{
	buf += Color::read(buf, backgroundColor);

	return Widget::read(buf);
}

std::string ListPanelItem::writeXml(int indent)
{
	std::stringstream str;

	str << Util::getIndent(indent) << "<ListPanelItem";
	writeXmlAttributes(str);
	str << ">" << std::endl;

	str << backgroundColor.writeXml(indent + 1, "BackgroundColor");

	writeXmlChildren(str, indent + 1);

	str << Util::getIndent(indent) << "</ListPanelItem>";
	return str.str();
}

uint8_t* ListPanel::read(uint8_t *buf)
{
	int tmp;
	buf += Util::readInt32(buf, tmp);
	scrollBarVisibility = static_cast<ScrollBarVisibility>(tmp);
	buf += Util::readBool(buf, showItemBorder);
	buf += Util::readBool(buf, showSection);
	buf += Util::readBool(buf, showEmptySection);
	buf += Util::readString(buf, listPanelItemName);
	return Widget::read(buf);
}

std::string ListPanel::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<ListPanel";
	writeXmlAttributes(str);
	str << ">" << std::endl;

	Util::writeXmlElement(str, indent + 1, "ScrollBarVisibility", scrollBarVisibility);
	Util::writeXmlElement(str, indent + 1, "ShowItemBorder", scrollBarVisibility);
	Util::writeXmlElement(str, indent + 1, "ShowSection", scrollBarVisibility);
	Util::writeXmlElement(str, indent + 1, "ShowEmptySection", scrollBarVisibility);
	Util::writeXmlElement(str, indent + 1, "ListPanelItemName", listPanelItemName);

	writeXmlChildren(str, indent + 1);
	
	str << Util::getIndent(indent) << "</ListPanel>";
	return str.str();
}

uint8_t *Panel::read(uint8_t* buf)
{
	buf += Color::read(buf, backgroundColor);
	return Widget::read(buf);
}

std::string Panel::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<Panel";
	writeXmlAttributes(str);

	str << ">" << std::endl;
	str << backgroundColor.writeXml(indent + 1, "BackgroundColor");
	
	writeXmlChildren(str, indent + 1);
	
	str << Util::getIndent(indent) << "</Panel>";
	return str.str();
}

std::string BusyIndicator::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<BusyIndicator";

	Util::writeXmlAttribute(str, "animating", animating);
	writeXmlAttributes(str);
	str << " />" << std::endl;
	return str.str();
}

uint8_t* SceneW::read(uint8_t* buf)
{
	buf += Util::readString(buf, title);
	return Widget::read(buf);
}

uint8_t* Label::read(uint8_t* buf)
{
	buf += Util::readString(buf, text);
	buf += Color::read(buf, textColor);
	buf += FontData::read(buf, font);
	
	int tmp;
	buf += Util::readInt32(buf, tmp);
	textTrimming = static_cast<TextTrimming>(tmp);
	buf += Util::readInt32(buf, tmp);
	lineBreak = static_cast<LineBreak>(tmp);
	buf += Util::readBool(buf, hasTextShadow);
	buf += TextShadow::read(buf, textShadow);
	buf += Util::readInt32(buf, tmp);
	horizontalAlignment = static_cast<HorizontalAlignment>(tmp);
	buf += Util::readInt32(buf, tmp);
	verticalAlignment = static_cast<VerticalAlignment>(tmp);

	buf += Util::readBool(buf, enableBaselineY);
	buf += Util::readFloat32(buf, topbaselineY);

	return Widget::read(buf);
}

std::string Label::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<Label";

	Widget::writeXmlAttributes(str);
	str << ">" << std::endl;

	Util::writeXmlElement(str, indent + 1, "Text", text);
	str << font.writeXml(indent + 1);
	Util::writeXmlElement(str, indent + 1, "TextTrimming", textTrimming);
	Util::writeXmlElement(str, indent + 1, "HasTextShadow", hasTextShadow);
	str << textShadow.writeXml(indent + 1);
	Util::writeXmlElement(str, indent + 1, "HorizontalAlignment", horizontalAlignment);
	Util::writeXmlElement(str, indent + 1, "VerticalAlignment", verticalAlignment);

	str << Util::getIndent(indent) << "</Label>";

	return str.str();
}

uint8_t* TextField::read(uint8_t* buf)
{
	buf += Util::readString(buf, text);
	buf += Util::readString(buf, placeholder);
	buf += FontData::read(buf, textFont);
	buf += Color::read(buf, textColor);

	int tmp;
	buf += Util::readInt32(buf, tmp);
	horizontalAlignment = static_cast<HorizontalAlignment>(tmp);
	buf += Util::readInt32(buf, tmp);
	verticalAlignment = static_cast<VerticalAlignment>(tmp);
	buf += Util::readInt32(buf, tmp);
	textTrimming = static_cast<TextTrimming>(tmp);
	buf += Util::readInt32(buf, tmp);
	lineBreak = static_cast<LineBreak>(tmp);
	buf += Util::readInt32(buf, textInputMode);
	buf += Util::readBool(buf, passwordMode);
	buf += Util::readInt32(buf, imeWindowMode);
	buf += Util::readString(buf, imeWindowTitle);
	buf += Util::readInt32(buf, imeEnterLabel);
	buf += Util::readBool(buf, acceptsReturn);
	buf += Color::read(buf, placeHolderColor);
	buf += FontData::read(buf, placeholderFont);

	return Widget::read(buf);
}

std::string TextField::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<TextField";
	writeXmlAttributes(str);
	str << ">" << std::endl;

	Util::writeXmlElement(str, indent + 1, "Text", text);
	str << textColor.writeXml(indent + 1, "TextColor");
	str << textFont.writeXml(indent + 1, "TextFontData");
	Util::writeXmlElement(str, indent + 1, "InputMode", textInputMode);
	Util::writeXmlElement(str, indent + 1, "PasswordMode", passwordMode);
	Util::writeXmlElement(str, indent + 1, "AcceptsReturn", acceptsReturn);
	Util::writeXmlElement(str, indent + 1, "ImeWindowTitle", imeWindowTitle);
	Util::writeXmlElement(str, indent + 1, "ImeEnterLabel", imeEnterLabel);
	Util::writeXmlElement(str, indent + 1, "ImeWindowMode", imeWindowMode);
	Util::writeXmlElement(str, indent + 1, "Placeholder", placeholder);
	str << placeHolderColor.writeXml(indent + 1, "PlaceHolderColor");
	str << placeholderFont.writeXml(indent + 1, "PlaceholderFont");
	Util::writeXmlElement(str, indent + 1, "TextTrimming", textTrimming);
	Util::writeXmlElement(str, indent + 1, "LineBreak", lineBreak);
	Util::writeXmlElement(str, indent + 1, "HorizontalAlignment", horizontalAlignment);
	Util::writeXmlElement(str, indent + 1, "VerticalAlignment", verticalAlignment);

	str << Util::getIndent(indent) << "</TextField>";

	return str.str();
}


std::string SceneW::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<Scene";

	Util::writeXmlAttribute(str, "title", title);
	writeXmlAttributes(str);

	str << ">" << std::endl;

	for (int i = 0; i < children.size(); i++)
	{
		str << children[i]->writeXml(indent + 1) << std::endl;
	}

	str << "</Scene>";
	return str.str();
}

uint8_t* ImageBox::read(uint8_t* buf)
{
	int tmp;
	buf += Util::readInt32(buf, tmp);
	imageScaleType = static_cast<ImageScaleType>(tmp);
	buf += NinePatchMargin::read(buf, ninePatchMargin);
	buf += Util::readString(buf, image);
	return Widget::read(buf);
}

std::string ImageBox::writeXml(int indent)
{
	std::stringstream str;
	str << Util::getIndent(indent) << "<ImageBox";
	writeXmlAttributes(str);
	Util::writeXmlAttribute(str, "imageScaleType", Util::toString(imageScaleType));
	str << ">" << std::endl;
	str << ninePatchMargin.writeXml(indent + 1);
	Util::writeXmlElement(str, indent + 1, "Image", image);

	Widget::writeXmlElements(str, indent + 1);

	if (numChildren <= 0)
	{
		str << Util::getIndent(indent) << "</ImageBox>";
		return str.str();
	}

	writeXmlChildren(str, indent);

	return str.str();
}

LayoutFile::LayoutFile(uint8_t* f)
{
	if (f == nullptr)
		return;

	layout_header header;

	memcpy(&header, f, sizeof(header));
	f = loadTypes(f + sizeof(header), header.typeCt);
}

void LayoutFile::processWidget(uint8_t* buf, Widget *widget)
{
	for (int i = 0; i < widget->numChildren; i++)
	{
		int index = 0;
		buf += Util::readInt32(buf, index);

		std::string name = types[index].name;

		if (name == "BusyIndicator")
		{
			BusyIndicator *bi = new BusyIndicator();
			buf = bi->read(buf);
			
			if (bi->numChildren > 0)
				processWidget(buf, bi);

			widget->children.push_back(bi);
			widgets.push_back(bi);
		}
		else if (name == "Button")
		{
			Button *button = new Button();
			buf = button->read(buf);

			if (button->numChildren > 0)
				processWidget(buf, button);

			widget->children.push_back(button);
			widgets.push_back(button);
		}
		else if (name == "Panel")
		{
			Panel *panel = new Panel();
			buf = panel->read(buf);

			if (panel->numChildren > 0)
				processWidget(buf, panel);

			widget->children.push_back(panel);
			widgets.push_back(panel);
		}
		else if (name == "Label")
		{
			Label *label = new Label();
			buf = label->read(buf);

			if (label->numChildren > 0)
				processWidget(buf, label);

			widget->children.push_back(label);
			widgets.push_back(label);
		}
		else if (name == "ImageBox")
		{
			ImageBox *imageBox = new ImageBox();
			buf = imageBox->read(buf);

			if (imageBox->numChildren > 0)
			{
				processWidget(buf, imageBox);
			}

			widget->children.push_back(imageBox);
			widgets.push_back(widget);
		}
		else if (name == "GridListPanel")
		{
			GridListPanel *panel = new GridListPanel();
			buf = panel->read(buf);

			if (panel->numChildren > 0)
				processWidget(buf, panel);

			widget->children.push_back(panel);
			widgets.push_back(panel);
		}
		else if (name == "ScrollPanel")
		{
			ScrollPanel *panel = new ScrollPanel();
			buf = panel->read(buf);

			if (panel->numChildren > 0)
				processWidget(buf, panel);

			widget->children.push_back(panel);
			widgets.push_back(panel);
		}
		else if (name == "ListPanel")
		{
			ListPanel *panel = new ListPanel();
			buf = panel->read(buf);

			if (panel->numChildren > 0)
				processWidget(buf, panel);

			widget->children.push_back(panel);
			widgets.push_back(panel);
		}
		else if (name == "TextField")
		{
			TextField *textField = new TextField();
			buf = textField->read(buf);

			if (textField->numChildren > 0)
				processWidget(buf, textField);

			widget->children.push_back(textField);
			widgets.push_back(textField);
		}
		else if (name == "ProgressBar")
		{
			ProgressBar *progressBar = new ProgressBar();
			buf = progressBar->read(buf);

			widget->children.push_back(progressBar);
			widgets.push_back(progressBar);
		}
		else
		{
			printf("UNSUPPORTED TYPE! [%s]\n", name.c_str());
			exit(1);
		}
	}
}

uint8_t *LayoutFile::loadTypes(uint8_t* buf, unsigned int count)
{
	if (buf == nullptr)
		return buf;

	for (int i = 0; i < count; i++)
	{
		LayoutType layoutType;

		buf += Util::readInt32(buf, layoutType.widgetIndex);
		buf += Util::readString(buf, layoutType.name);

		types.push_back(layoutType);
	}

	int rootWidget;
	buf += Util::readInt32(buf, rootWidget);

	if (types[rootWidget].name == "Scene")
	{
		root = new SceneW();
	}
	else if (types[rootWidget].name == "Panel")
	{
		root = new Panel();
	}
	else if (types[rootWidget].name == "ListPanelItem")
	{
		root = new ListPanelItem();
	}
	if (root == nullptr)
	{
		printf("Unsupported root widget\r\n");
		return buf;
	}

	buf = root->read(buf);

	for (int i = 0; i < root->numChildren; i++)
	{
		processWidget(buf, root);
	}

	return buf;
}

void LayoutFile::write(FILE *f)
{
	if (root == nullptr)
		return;
	std::string xml = root->writeXml(0);
	unsigned int len = xml.length();
	fwrite(xml.c_str(), sizeof(char), len, f);
	fflush(f);
}

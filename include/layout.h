#pragma once
#include <cstdio>
#include <string>
#include "rco.h"

#define LAYOUT_MAX_STR 255

enum ProgressBarType
{
	BarNormal,
	BarAnimation,
	BarRigid
};

enum ScrollBarVisibility
{
	Visible,
	ScrollableVisible,
	ScrollingVisible,
	Invisible
};

enum GridListScrollOrientation
{
	GridHorizontal,
	GridVertical
};

enum ImageScaleType
{
	ScaleCenter = 0,
	ScaleStretch,
	ScaleAspectInside,
	ScaleAspectOutside,
	ScaleNinePatch
};

enum Type
{
	SceneType,
	DialogType,
	PanelType,
	ListPanelItemType,
	Unknown
};

enum Anchor
{
	AnchorNone = 0,
	AnchorTop = 1,
	AnchorBottom = 2,
	AnchorHeight = 4,
	AnchorLeft = 16,
	AnchorRight = 32,
	AnchorWidth = 64
};

enum HorizontalAlignment
{
	HAlignLeft = 0,
	HAlignCenter,
	HAlignRight,
};

enum VerticalAlignment
{
	VAlignTop = 0,
	VAlignBottom,
	VAlignCenter
};

enum TextTrimming
{
	TrimNone = 0,
	TrimCharacter,
	TrimWord,
	TrimEllipsisCharacter,
	TrimEllipsisWord,
	TrimMarquee,
	TrimRoundMarquee
};

enum LineBreak
{
	BreakCharacter = 0,
	BreakWord,
	BreakBreakableSymbol,
	BreakAtCode
};

struct Transform
{
	float transX;
	float transY;
	float transZ;
	float scaleX;
	float scaleY;
	float scaleZ;
	float angleX;
	float angleY;
	float angleZ;

	static int read(uint8_t *buf, Transform &out);
	std::string writeXmlAttribute() const;
};

struct Color
{
	float r;
	float g;
	float b;
	float a;
	
	static int read(uint8_t *buf, Color &out);
	std::string writeXml(int indent, std::string name = "Color") const;
};

struct TextShadow
{
	Color color;
	float xOffset;
	float yOffset;

	static int read(uint8_t *buf, TextShadow &out);
	std::string writeXml(int indent) const;
};

struct NinePatchMargin
{
	int top;
	int bottom;
	int left;
	int right;

	static int read(uint8_t *buf, NinePatchMargin &out);
	std::string writeXml(int indent) const;
};

struct FontData
{
	int fontSize;
	int fontStyle;
	int fontWeight;
	std::string fontFamily;

	static int read(uint8_t *buf, FontData &out);
	std::string writeXml(int indent, std::string name = "FontData") const;
};

struct layout_header
{
	uint8_t magic[4]; // UIBL
	uint32_t version; // 0003
	uint32_t typeCt; // Number of types in the type table
};

class Widget
{
public:
	int numChildren;
	std::vector<Widget *> children;

	virtual std::string writeXml(int indent) = 0;
	virtual uint8_t *read(uint8_t *buf);

protected:
	int type;	
	float x;
	float y;
	float width;
	float height;
	bool visible;
	std::string name;
	std::string stylefile;
	Anchor anchor;
	Transform transform;	

	void writeXmlAttributes(std::stringstream &str) const;
	void writeXmlElements(std::stringstream &str, int indent) const;
	void writeXmlChildren(std::stringstream &str, int indent) const;
};

class BusyIndicator : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;
private:
	bool animating = false;	
};

class ProgressBar : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	ProgressBarType progressBarType;
	float progress;
};

class Button : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;
private:
	int textTrimming;
	bool isTextButton;
	bool isCustomBackground;
	bool hasTextShadow;
	std::string text;
	std::string backgroundNormalImage;
	std::string backgroundPressedImage;
	std::string backgroundDisabledImage;
	std::string iconImage;
	Color textColor;
	FontData textFont;
	TextShadow textShadow;
	NinePatchMargin margin;
	Color backgroundFilter;
};

class Label : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	bool hasTextShadow;
	bool enableBaselineY;
	float topbaselineY;
	std::string text = "";
	Color textColor;
	FontData font;
	TextTrimming textTrimming;
	LineBreak lineBreak;
	TextShadow textShadow;
	HorizontalAlignment horizontalAlignment;
	VerticalAlignment verticalAlignment;
};

class TextField : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;
	
private:
	bool passwordMode;
	bool acceptsReturn;
	int textInputMode;
	int imeWindowMode;
	int imeEnterLabel;
	std::string imeWindowTitle;
	std::string text;
	std::string placeholder;
	Color placeHolderColor;
	Color textColor;
	FontData placeholderFont;
	FontData textFont;
	HorizontalAlignment horizontalAlignment;
	VerticalAlignment verticalAlignment;
	TextTrimming textTrimming;
	LineBreak lineBreak;
};

class ImageBox : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	std::string image;
	ImageScaleType imageScaleType;
	NinePatchMargin ninePatchMargin;
};

class GridListPanel : public Widget
{
public: 
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	float itemWidth;
	float itemHeight;
	std::string listPanelItemName;	
	ScrollBarVisibility scrollbarVisibility;
	GridListScrollOrientation scrollOrientation;
};

class ScrollPanel : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	bool horizontalScroll;
	bool verticalScroll;
	float panelX;
	float panelY;
	float panelWidth;
	float panelHeight;
	std::string panelName;
	ScrollBarVisibility scrollBarVisibility;
};

class ListPanelItem : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	Color backgroundColor;
};

class ListPanel : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	bool showItemBorder;
	bool showSection;
	bool showEmptySection;
	std::string listPanelItemName;
	ScrollBarVisibility scrollBarVisibility;
};

class Panel : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;

private:
	Color backgroundColor;
};

class SceneW : public Widget
{
public:
	uint8_t *read(uint8_t *buf) override;
	std::string writeXml(int indent) override;
	private:
	std::string title;
};

class LayoutType
{
public:
	LayoutType() : LayoutType(0, "") {}
	LayoutType(int typeId, std::string name) : widgetIndex(typeId), name(name) {}
	int widgetIndex;
	std::string name;
};

class LayoutFile
{
public:
	LayoutFile(uint8_t *f);
	void write(FILE *f);

private:
	void processWidget(uint8_t *buf, Widget *widget);
	std::vector<LayoutType> types;
	std::vector<Widget*> widgets;
	uint8_t *loadTypes(uint8_t *buf, unsigned int type);
	std::string name;

	Widget *root = nullptr;
};
#pragma once

#include "ofMain.h"

#define OF_COLOR_RGB 0
#define OF_COLOR_HSV 1

#define OF_COLOR_SORT_RED 0
#define OF_COLOR_SORT_GREEN 1
#define OF_COLOR_SORT_BLUE 2
#define OF_COLOR_SORT_HUE 3
#define OF_COLOR_SORT_SATURATION 4
#define OF_COLOR_SORT_VALUE 5
#define OF_COLOR_SORT_ALPHA 6

class ofxColor {
public:
	static void setColorMode(int colorMode);
	static int getColorMode();
	static void setColorRange(float colorRange);
	static float getColorRange();
	static void setColorSortMode(int colorSortMode);
	static int getColorSortMode();

	float
		alpha,
		red, green, blue,
		hue, saturation, value;

	ofxColor();
	ofxColor(const ofxColor& x);
	ofxColor(unsigned int asHex, int colorMode = ofxColor::colorMode);
	ofxColor(float x, float y, float z, int colorMode = ofxColor::colorMode);
	ofxColor& operator=(const ofxColor& x);
	ofxColor operator+(const ofxColor& x) const;
	int operator==(const ofxColor& x) const;
	bool operator<(const ofxColor& x) const;
	bool operator>(const ofxColor& x) const;
	float distance(const ofxColor& x, int colorMode = ofxColor::colorMode) const;
	unsigned int asHex(int colorMode = ofxColor::colorMode) const;
	void update();
	void clear();
	void updateRgb();
	void updateHsv();
	void setColor() const;
	virtual ~ofxColor();
private:
	static int colorMode;
	static float colorRange;
	static int colorSortMode;
};

void ofSetColor(const ofxColor& color);

// <3
// kyle mcdonald

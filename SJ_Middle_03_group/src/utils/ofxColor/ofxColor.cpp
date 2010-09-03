#include "ofxColor.h"

#define getByte(x, y) ((x >> (8 * y)) & 0xff)
#define makeColor(a, x, y, z, r) \
	((((unsigned char) (a * r) & 0xff) << 24) | \
	(((unsigned char) (x * r) & 0xff) << 16) | \
	(((unsigned char) (y * r) & 0xff) << 8) | \
	((unsigned char) (z * r) & 0xff))

int ofxColor::colorMode = OF_COLOR_RGB;
float ofxColor::colorRange = 255.;
int ofxColor::colorSortMode = OF_COLOR_SORT_VALUE;

void ofxColor::setColorMode(int colorMode) {
	ofxColor::colorMode = colorMode;
}

int ofxColor::getColorMode() {
	return colorMode;
}

void ofxColor::setColorRange(float colorRange) {
	ofxColor::colorRange = colorRange;
}

float ofxColor::getColorRange() {
	return colorRange;
}

void ofxColor::setColorSortMode(int colorSortMode) {
	ofxColor::colorSortMode = colorSortMode;
}

int ofxColor::getColorSortMode() {
	return colorSortMode;
}

ofxColor::ofxColor() {
	clear();
}

ofxColor::ofxColor(const ofxColor& x) {
	(*this) = x;
}

ofxColor::ofxColor(unsigned int asHex, int colorMode) {
	float resize = colorRange / 255.;
	alpha = getByte(asHex, 3) * resize;
	if(colorMode == OF_COLOR_RGB) {
		red = getByte(asHex, 2) * resize;
		green = getByte(asHex, 1) * resize;
		blue = getByte(asHex, 0) * resize;
		updateHsv();
	} else if(colorMode == OF_COLOR_HSV) {
		red = getByte(asHex, 2) * resize;
		green = getByte(asHex, 1) * resize;
		blue = getByte(asHex, 0) * resize;
		updateRgb();
	}
	alpha = colorRange;
}

ofxColor::ofxColor(float x, float y, float z, int colorMode) {
	if(colorMode == OF_COLOR_RGB) {
		red = x;
		green = y;
		blue = z;
		updateHsv();
	} else if(colorMode == OF_COLOR_HSV) {
		hue = x;
		saturation = y;
		value = z;
		updateRgb();
	}
	this->alpha = colorRange;
}

ofxColor ofxColor::operator+(const ofxColor& x) const {
	ofxColor both;
	if(colorMode == OF_COLOR_RGB) {
		both.red = red + x.red;
		both.green = green + x.green;
		both.blue = blue + x.blue;
		both.updateHsv();
	} else if(colorMode == OF_COLOR_HSV) {
		both.hue = hue + x.hue;
		both.saturation = saturation + x.saturation;
		both.value = value + x.value;
		both.updateRgb();
	}
	return both;
}

ofxColor& ofxColor::operator=(const ofxColor& x) {
	red = x.red;
	green = x.green;
	blue = x.blue;
	hue = x.hue;
	saturation = x.saturation;
	value = x.value;
	alpha = x.alpha;
	return *this;
}

int ofxColor::operator==(const ofxColor& x) const {
	if(red == x.red &&
		green == x.green &&
		blue == x.blue)
		return 0;
	switch(colorSortMode) {
		case OF_COLOR_SORT_RED: return red < x.red ? -1 : +1;
		case OF_COLOR_SORT_GREEN: return green < x.green ? -1 : +1;
		case OF_COLOR_SORT_BLUE: return blue < x.blue ? -1 : +1;
		case OF_COLOR_SORT_HUE: return hue < x.hue ? -1 : +1;
		case OF_COLOR_SORT_SATURATION: return saturation < x.saturation ? -1 : +1;
		case OF_COLOR_SORT_VALUE: return value < x.value ? -1 : +1;
		case OF_COLOR_SORT_ALPHA: return alpha < x.alpha ? -1 : +1;
	}
	return 0;
}

bool ofxColor::operator<(const ofxColor& x) const {
	return (*this == x) < 0;
}

bool ofxColor::operator>(const ofxColor& x) const {
	return (*this == x) > 0;
}

float ofxColor::distance(const ofxColor& x, int colorMode) const {
	float maxDistance = sqrtf(3 * colorRange * colorRange);
	if(colorMode == OF_COLOR_RGB) {
		float rr = red - x.red;
		float gg = green - x.green;
		float bb = blue - x.blue;
		return sqrtf(rr * rr + gg * gg + bb * bb) / maxDistance;
	} else if(colorMode == OF_COLOR_HSV) {
		float hh = hue - x.hue;
		float ss = saturation - x.saturation;
		float bb = value - x.value;
		return sqrtf(hh * hh + ss * ss + bb * bb) / maxDistance;
	}
}

unsigned int ofxColor::asHex(int colorMode) const {
	float resize = 255. / colorRange;
	if(colorMode == OF_COLOR_RGB) {
		return makeColor(alpha, red, green, blue, resize);
	} else if(colorMode == OF_COLOR_HSV) {
		return makeColor(alpha, hue, saturation, value, resize);
	}
}

void ofxColor::clear() {
	red = 0;
	green = 0;
	blue = 0;
	hue = 0;
	saturation = 0;
	value = 0;
	alpha = colorRange;
}

void ofxColor::update() {
	if(colorMode == OF_COLOR_RGB)
		updateHsv();
	else if(colorMode == OF_COLOR_HSV)
		updateRgb();
}

void ofxColor::setColor() const {
	float resize = 255. / getColorRange();
	if(alpha != 0)
		ofSetColor(
			(int) (red * resize),
			(int) (green * resize),
			(int) (blue * resize),
			(int) (alpha * resize));
	else
		ofSetColor(
			(int) (red * resize),
			(int) (green * resize),
			(int) (blue * resize));
}

void ofSetColor(const ofxColor& color) {
	color.setColor();
}

ofxColor::~ofxColor() {
}

void ofxColor::updateRgb() {
	if(value == 0) {
		red = 0;
		green = 0;
		blue = 0;
	} else if(saturation == 0) {
		red = value;
		green = value;
		blue = value;
	} else {
		float normalHue = (float) hue / (colorRange / 6.);
		float normalSaturation = (float) saturation / colorRange;
		int hueCategory = (int) floor(normalHue);
		float hueRemainder = normalHue - hueCategory;
		float pv = (1. - normalSaturation) * value;
		float qv = (1. - normalSaturation * hueRemainder) * value;
		float tv = (1. - normalSaturation * (1. - hueRemainder)) * value;
		switch(hueCategory) {
			case 0: // red
				red = value;
				green = tv;
				blue =  pv;
				break;
			case 1: // green
				red = qv;
				green = value;
				blue = pv;
				break;
			case 2:
				red = pv;
				green = value;
				blue = tv;
				break;
			case 3: // blue
				red = pv;
				green = qv;
				blue = value;
				break;
			case 4:
				red = tv;
				green = pv;
				blue = value;
				break;
			case 5: // back to red
				red = value;
				green = pv;
				blue = qv;
				break;
		}
	}
}
void ofxColor::updateHsv() {
	float nred = red / colorRange;
	float ngreen = green / colorRange;
	float nblue = blue / colorRange;

	float max = nred;
	if(ngreen > max)
		max = ngreen;
	if(nblue > max)
		max = nblue;

	float min = nred;
	if(ngreen < min)
		min = ngreen;
	if(nblue < min)
		min = nblue;

	if(max == min) { // greys
		hue = 0;
		saturation = 0;
		value = max * colorRange;
		return;
	} else {
		value = max;
		saturation = (max - min) / max;
	}

	if(nred == max) {
		hue = 0. + (ngreen - nblue) / (max - min);
		if(hue < 0)
			hue += 6.;
	} else if (ngreen == max) {
		hue = 2. + (nblue - nred) / (max - min);
	} else {
		hue = 4. + (nred - ngreen) / (max - min);
	}

	hue *= (colorRange / 6.);
	saturation *= colorRange;
	value *= colorRange;
}

/*
 *  LineGenerator.h
 *  orbitalExperiments
 *
 *  Created by Jim on 7/23/10.
 *  Copyright 2010 University of Washington. All rights reserved.
 *
 */


#ifndef LINE_GENERATOR
#define LINE_GENERATOR

#include "ofMain.h"
#include "ofxVectorMath.h"
#include "particle.h"
#include "ofxMSASpline.h"
#include "ofxFBOTexture.h"
#include "ofShader.h"

struct LineNode {
	ofxPoint2f pos;
	ofxPoint2f original;
	LineNode* prev;
	float width;	
};

class LineGenerator {

  public:
	void setup(int lines, int nodes);
	void update(vector<particle>& points);	
	void draw();
	vector<LineNode> nodes;
	ofxMSASpline2D spline;
	ofxFBOTexture mask;
	
	ofImage large;
	ofShader shader;
};

#endif
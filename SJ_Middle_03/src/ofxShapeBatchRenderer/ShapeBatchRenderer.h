/*
 *  ShapeBatchRenderer.h
 *
 *  Created by Zach Gage on 6/21/10.
 *  Copyright 2010 stfj. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

#define DEFAULT_LAYER 0

enum SHAPEMODE_t {SBR_LINE=2, SBR_TRIANGLE=3}; //line or triangle based drawing.... essentially ofNoFill(), or ofFill();

//class -------------------------------------------
class ShapeBatchRenderer
{
public:
	ShapeBatchRenderer(SHAPEMODE_t mode, int _numObjectsPerLayer, int _numLayers);
	~ShapeBatchRenderer();
	
	//raw --
		//this can only be used if you're in triangle mode
		bool addTriangle(ofPoint a, ofPoint b, ofPoint c, int layer=DEFAULT_LAYER);
		bool addTriangle(ofPoint a, int aR, int aG, int aB, int aA, ofPoint b, int bR, int bG, int bB, int bA, ofPoint c, int cR, int cG, int cB, int cA, int layer=DEFAULT_LAYER);
		
		//this can only be used if you're in line mode
		bool addLine(ofPoint a, ofPoint b, int layer=DEFAULT_LAYER);
		bool addLine(ofPoint a, int aR, int aG, int aB, int aA, ofPoint b, int bR, int bG, int bB, int bA, int layer=DEFAULT_LAYER);
	
	
	//handled --
		bool addCircle(float x, float y, float z, float radius, int layer=DEFAULT_LAYER);
		bool addElipse(float x, float y, float z, float w, float h, int layer=DEFAULT_LAYER);
		
		bool addRect(float x, float y, float z, float w, float h, int layer=DEFAULT_LAYER);
		bool addCenteredRect(float x, float y, float z, float w, float h, int layer=DEFAULT_LAYER);
	
	//functional --
		void draw();
		void clear();
	
	//features --
		void setColor(int r, int g, int b, int a=255);
		void setCircleResolution(int resolution);
		void setLayerLineWidth(int layer, int lineWidth); //only relavent in SBR_LINE mode
		void setSafeMode(bool safe);
		
protected:	
	float * verts;
	unsigned char * colors;
		
	int * numObjects;
	int * layerLineWidth;
	
	int numLayers;
	int numObjectsPerLayer;
	
	bool bSetupCircle;
	
	bool safeMode;
				   
	SHAPEMODE_t shapeMode;
	
	int red;
	int green;
	int blue;
	int alpha;
	
	int	numCirclePts;
	float circlePts[OF_MAX_CIRCLE_PTS*2];
};
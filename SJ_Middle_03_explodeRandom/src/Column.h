/*
 *  Column.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 8/24/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 
 this is a bare-bones class that keeps track of columns 
 particles move towards
 
 */

#pragma once

#include "ofMain.h"

class Column : public ofRectangle
{
public:

	int lastPicked;
	Column(){ lastPicked = 0; };
	
};
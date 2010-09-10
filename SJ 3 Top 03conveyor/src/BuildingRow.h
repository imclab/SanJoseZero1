/*
 *  BuildingRow.h
 *  SJ Stack 02
 *
 *  Created by brenfer on 7/8/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "Globals.h"
#include "Stack.h"
#include "Building.h"
#include "BuildingRow.h"

#include "LCurve.h"

class BuildingRow
{
public:
	
	BuildingRow(){
		setup();
	};
	
	BuildingRow(float _pos){
		setBasePosition( _pos );
		setup();
	};
	
	void setup(){
		bComplete = false;
		color.r = 50;//ofRandom(0, 255);
		color.g = 50;//ofRandom(0, 255);
		color.b = 50;//ofRandom(0, 255);
		pos.x = 0;//ofGetWidth()/2.0;
		pos.y = 0;//ofGetHeight()/2.0;
		rotation.x = 270;
		size.x = screenWidth;
		size.y = 70;
		
		//build base columns
		float curX = ROW_BUFFER + ROW_SPACING;
					
		for (int i=0; i<NUMBER_OF_ROWS; i++){
			Stack * s = new Stack(i);
			s->setPosition(curX, pos.z);
			stacks.push_back(s);
			curX += ROW_SPACING;
		};
	}
	
	void setBasePosition( float _pos ){
		pos.z = _pos;
		
		for (int i=0; i<stacks.size(); i++){
			stacks[i]->setPosition(stacks[i]->getPosition().x, pos.z);
		}
		
	};
	
	float getPosition(){
		return pos.z;
	}
	
	void setHeight( float height ){
		size.y = height;
	}
	
	ofPoint getSize(){
		return size;
	}
	
	float getCeiling(){
		return ceiling;
	}
	
	float getCeiling( int which ){
		return stacks[which]->getHeight() + pos.y + pos.z;
	}
	
	Stack * getClosestStack( float position ){
		float closest = 999999;
		int columnIndex = -1;
		for (int i=0; i<stacks.size(); i++){
			float dist = fabs(position - stacks[i]->getPosition().x);
			if ( dist < closest){
				closest = dist;
				columnIndex = i;
			};
		};
		
		return stacks[columnIndex];
	};
	
	void addToStack( Building * b ){
		stacks[ b->getWhichStack() ]->addBuilding(b);
	};
	
	void update(){
		
		//update ceiling
		ceiling = 0;
		
		for (int i=0; i<stacks.size(); i++){
			stacks[i]->update();
			if (stacks[i]->getHeight() > ceiling) 
				ceiling = stacks[i]->getHeight();
		};
		ceiling += pos.y + pos.z;
	};
	
	void draw(){
		ofPushMatrix();{
			ofTranslate(pos.x, pos.y, 0);
			for (int i=0; i<stacks.size(); i++){
				stacks[i]->draw();
			};
		} ofPopMatrix();
	};
	
	void windowResized(){
		pos.y = 100;//ofGetHeight()/2.0;
		size.x = ofGetWidth();
	};
	
	void setComplete( bool _bComplete ){
		bComplete = _bComplete;
		
		//reset all building scales to 1:1:1
		for (int i=0; i<stacks.size(); i++){
			stacks[i]->setComplete();
		}		
	}
	
	ofPoint rotation;
	ofPoint pos;
	ofPoint size;
	ofColor color;
	vector <Stack *> stacks;
	
protected:
	float ceiling;
	bool bComplete;
};

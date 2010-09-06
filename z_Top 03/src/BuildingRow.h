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

class BuildingRow
{
public:
	
	BuildingRow(){
		setup();
	};
	
	BuildingRow(float _pos){
		setup();
		setBasePosition( _pos );
	};
	
	void setup(){
		bComplete = false;
		color.r = 50;//ofRandom(0, 255);
		color.g = 50;//ofRandom(0, 255);
		color.b = 50;//ofRandom(0, 255);
		pos.x = 0;//ofGetWidth()/2.0;
		pos.y = 100;//ofGetHeight()/2.0;
		rotation.x = 270;
		size.x = ofGetWidth();
		size.y = 70;
		
		//build base columns
		float increment = (float)(ofGetWidth()-ROW_BUFFER*2)/NUMBER_OF_ROWS;
		
		for (int i=0; i<NUMBER_OF_ROWS; i++){
			Stack * s = new Stack(i);
			s->setPosition(ROW_BUFFER + i*increment, 0);
			stacks.push_back(s);
		};
	}
	
	void setBasePosition( float _pos ){
		pos.z = _pos;
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
			if ( dist <closest){
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
			ofRotateX( rotation.x );
			ofRotateY( rotation.y );
			ofRotateZ( rotation.z );
			ofTranslate(0, -size.y/2.0, pos.z);
			
			//draw floor
			ofSetColor(color.r, color.g, color.b);
			//ofSetColor(0x33333);
			ofRect(0,0,size.x, size.y);
			if (bComplete)
				ofSetColor(255,255,255,255);
			else ofSetColor(0xffffff);
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
		for (int i=0; i<stacks.size(); i++){
			stacks[i]->setBuildingScale(1., 1.,1.);
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
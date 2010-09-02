/*
 *  Stack.h
 *  SJ Stack 02
 *
 *  Created by brenfer on 7/8/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "Building.h"
#include "LCurve.h"

class Stack
{
public:
	
	Stack(){
		index = 0;
		height = 0;
		rotAxis.set(1,0,0);
	};
	
	Stack( int _index ){
		index = _index;
		height = 0;
	};
	
	
	void setPosition( float x, float y ){
		pos.x = x;
		pos.y = y;
	};
	
	void setPosition( ofxVec3f newPos){	//LARS//
		//newPos.y += buildings[0]->getWidth();//<----//this centers the stack,
													//ultimately this should be fixed
													//in the models. The buildings base
													//should be centered at 0,0,0
		pos = newPos;					//LARS//
	};									//LARS//
	
	ofPoint getPosition(){
		return pos;
	};
	
	float getHeight(){
		return height;
	};
	
	void update(){
		height = 0;
		for (int i=0; i<buildings.size(); i++){
			height += buildings[i]->getHeight();
			width = fmax(width, buildings[i]->getWidth());
		};	 
	};
	
	void draw(){
		ofPushMatrix();{
			ofTranslate(pos.x, pos.y, pos.z);
			ofRotate(angle, rotAxis.x, rotAxis.y, rotAxis.z);
			//ofTranslate(0, buildings[0]->getWidth(), 0);
			ofTranslate(0.0, -width, 0.0);
			for (int i=0; i<buildings.size(); i++){
				//ofPushMatrix();{ //LARS//
					buildings[i]->draw();
				//} ofPopMatrix(); //LARS//
			};
		} ofPopMatrix();
	};
	
	void addBuilding( Building * b ){
		
		//update position
		b->rotate.x += -270;
		b->rotate.y += 0;
		b->setPos(0,b->getWidth(),height + b->getHeight()/2.0);
		buildings.push_back(b);
		update();
	};
	
	int index;
	vector <Building *> buildings;	
	
	ofxVec3f rotAxis;
	float angle;
protected:
	ofPoint pos;
	float height;
	float width;
};

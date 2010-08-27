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

class Stack
{
public:
	
	Stack(){
		index = 0;
		height = 0;
	};
	
	Stack( int _index ){
		index = _index;
		height = 0;
	};
	
	
	void setPosition( float x, float y ){
		pos.x = x;
		pos.y = y;
	};
	
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
		};	 
	};
	
	void draw(){
		ofPushMatrix();{
			ofTranslate(pos.x, pos.y, pos.z);
			for (int i=0; i<buildings.size(); i++){
				ofPushMatrix();{
					buildings[i]->draw();
				} ofPopMatrix();
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
protected:
	ofPoint pos;
	float height;
};

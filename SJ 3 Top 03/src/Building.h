/*
 *  Building.h
 *  SimpleSjCatcher
 *
 *  Created by brenfer on 6/30/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once
#include "Globals.h"
#include "ofx3DModelLoader.h"

class Building
{
public:
	
	Building(){
		bAlive = true;
		ceiling = 0;
		index = 0;
		rotateAround = 0;
		rotate.x = 5;
		//rotate = ofRandom(0, 360);
	}
	
	void setType(string _type){
		type = _type;
	}
	
	string getType (){
		return type;
	}
	
	// FIX THIS!!!
	
	float getHeight(){
		return 10*img->scale.x;
		//return img->width;
	}
	
	void setImage( ofx3DModelLoader * _img){
		img = _img;
		position.y += getHeight()*2;
	}
	
	void setPos( float x, float y, float z){
		position.x = x;
		position.y = y;
		position.z = z;
	}
	
	void setPos( ofPoint _pos ){
		position.x = _pos.x;
		position.y = _pos.y;
	}
	
	ofPoint getPosition(){
		return position;
	}
	
	void setCeiling( float ceil){
		ceiling = ceil;
	}
	
	void setStackIndex( int index ){
		whichStack = index;
	}
	
	int getWhichStack(){
		return whichStack;
	}
	
	void update(){
		position.y -= SPEED;
		//rotate += SPEED * .5;
		if (position.y - getHeight()/2 <= ceiling){
			position.y = ceiling;
			bAlive = false;
		}
	}
	void draw(){ 
		ofPushMatrix();{
			ofTranslate(position.x, position.y, position.z);
			ofTranslate(- getHeight()/2.0,  - getHeight()/2.0);
			
			ofRotateX(rotate.x);
			ofRotateY(rotate.y);
			ofRotateZ(rotate.z);
			//ofRotateX(5);
			//ofRotateZ(5);
			//ofRotateY(15);

			ofTranslate(0, getHeight()/2.0, 0);
			img->draw();
		} ofPopMatrix();
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint rotate;
	float rotateAround;
	int index;
private:
	ofPoint position;
	string type;
	
	bool bAlive;
	ofx3DModelLoader * img;
	float ceiling;
	int whichStack;
	
};

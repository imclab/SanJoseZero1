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
		rotate.x = 0;
		//rotate = ofRandom(0, 360);
		scale.z = .1;
		scale.x = scale.y = 1.0f;
	}
	
	string getData(){
		return data;
	};
	
	void setType(string _type){
		type = _type;
	}
	
	string getType (){
		return type;
	}
	
	float getHeight(){
		return img->getYdim()*img->scale.x;
		//return img->width;
	}
	
	float getWidth(){
		return img->getXdim()*img->scale.x;
		//return img->width;
	}
	
	float getDepth(){
		return img->getZdim()*img->scale.x;
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
		//rotate.x += SPEED * .5;
		//rotate.y += SPEED * .5;
		if (position.y - getHeight()/2 <= ceiling){
			position.y = ceiling;
			bAlive = false;
		}
	}
	void draw(){ 
		ofPushMatrix();{
			ofTranslate(position.x, position.y, position.z);
			ofScale(scale.x, scale.y, scale.z);
			//ofTranslate(getHeight()/2.0, getHeight()/2.0, getHeight()/2.0);
			
			ofRotateX(rotate.x);
			ofRotateY(rotate.y);
			ofRotateZ(rotate.z);
			
			//ofTranslate(-getHeight()/2.0,  -getHeight()/2.0, -getHeight()/2.0);
			//ofRotateX(5);
			//ofRotateZ(5);
			//ofRotateY(15);
			img->draw();
		} ofPopMatrix();
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint rotate;
	ofPoint scale;
	float rotateAround;
	int index;
private:
	ofPoint position;
	string type;
	string data;
	
	bool bAlive;
	ofx3DModelLoader * img;
	float ceiling;
	int whichStack;
	
};

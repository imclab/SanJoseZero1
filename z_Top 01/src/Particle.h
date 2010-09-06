/*
 *  Particle.h
 *  SimpleSjCatcher
 *
 *  Created by brenfer on 6/30/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#define SPEED 2
#include "ofx3DModelLoader.h"

class Particle
{
public:
	
	Particle(){
		bAlive = true;
		ceiling = 0;
		index = 0;
		rotateAround = 0;
		rotate.x = 5;
		//rotate = ofRandom(0, 360);
	}
	
	// FIX THIS!!!
	
	float getWidth(){
		return 10*img->scale.x;
		//return img->width;
	}
	
	void setImage( ofx3DModelLoader * _img){
		img = _img;
		loc.y += getWidth()*2;
	}
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	void setCeiling( float ceil){
		ceiling = ceil;
	}
	
	void update(){
		loc.y -= SPEED;
		//rotate += SPEED * .5;
		if (loc.y <= ceiling){
			loc.y = ceiling;
			bAlive = false;
		}
	}
	void draw(){ 
		ofPushMatrix();{
			ofTranslate(loc.x + getWidth()/2.0, loc.y - getWidth()/2.0);
			
			ofRotateX(rotate.x);
			ofRotateY(rotate.y);
			ofRotateZ(rotate.z);
			//ofRotateX(5);
			//ofRotateZ(5);
			//ofRotateY(15);

			ofTranslate(-getWidth()/2.0, getWidth() + getWidth()*index);
			img->draw();
		} ofPopMatrix();
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint loc;
	ofPoint rotate;
	float rotateAround;
	int index;
private:
	
	bool bAlive;
	ofx3DModelLoader * img;
	float ceiling;
	
};

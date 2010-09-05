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


class Particle
{
public:
	
	Particle(){
		bAlive = true;
		scale = 1.0f;
	}
	
	int getWidth(){
		return img->width;
	}
	
	void setImage( ofImage * _img){
		img = _img;
	}
	
	void setScale( float _scale){
		scale = _scale;
	}
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	void update(){
		loc.y -= SPEED;
		if (loc.y + img->height <= 0) bAlive = false;
	}
	void draw(){ 
		ofPushMatrix();{
			ofTranslate(loc.x, loc.y);
			ofScale(scale, scale, scale);
			img->draw(-getWidth()/2.0,-getWidth()/2.0);
		} ofPopMatrix();
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint loc;
private:
	
	bool bAlive;
	ofImage * img;
	float scale;
	
	
};

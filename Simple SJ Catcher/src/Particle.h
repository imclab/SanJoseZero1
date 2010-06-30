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
	}
	
	int getWidth(){
		return img->width;
	}
	
	void setImage( ofImage * _img){
		img = _img;
	}
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	void update(){
		loc.y -= SPEED;
		if (loc.y + img->height < 0) bAlive = false;
	}
	void draw(){ 
		img->draw(loc.x, loc.y);
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint loc;
private:
	
	bool bAlive;
	ofImage * img;
	
	
};

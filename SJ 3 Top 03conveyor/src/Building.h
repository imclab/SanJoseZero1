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
	
	
	float speed;
	
	Building(){
		bAlive = true;
		ceiling = 0;
		index = 0;
		rotateAround = 0;
		rotate.x = 0;
		//rotate = ofRandom(0, 360);
		scale.x = scale.y = 1.0f;
		scale.z = .01f;
		speed = -5.0f;
	}
	
	void setData( string _data ){
		data = _data;
	};
	
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
		img->setScale(1.0,1.0,1.0);
		return img->getYdim()*scale.y;
		//return img->width;
	}
	
	float getWidth(){
		img->setScale(1.0,1.0,1.0);
		return img->getXdim()*scale.x;
		//return img->width;
	}
	
	float getDepth(){
		img->setScale(1.0,1.0,1.0);
		return img->getZdim()*scale.z;
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
	
	void setScale( float x, float y, float z){
		setScaleX(x);
		setScaleY(y);
		setScaleZ(z);
	}
	
	void setScaleX(float x){
		scale.x = x;
	}
	void setScaleY(float y){
		scale.y = y;
	}
	void setScaleZ(float z){
		scale.z = z;
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
		//if (bAlive) cout<<"my speed is "<<speed<<endl;
		position.y += speed;
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
			ofRotateX(rotate.x);
			ofRotateY(rotate.y);
			ofRotateZ(rotate.z);
			ofScale(scale.x, scale.y, scale.z);
			img->draw();			
			
		} ofPopMatrix();
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint rotate;
	float rotateAround;
	int index;
	
	ofPoint scale;	
private:
	ofPoint position;
	string type;
	string data;
	
	bool bAlive;
	ofx3DModelLoader * img;
	float ceiling;
	int whichStack;
	
};

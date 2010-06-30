/*
 *  Emitter.h
 *  SimpleSjCatcher
 *
 *  Created by brenfer on 6/30/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "Particle.h"

#define EMITTER_TIME 1000

class Emitter
{
public:
	Emitter(){
		lastEmitted = ofGetElapsedTimeMillis();
		lastFoundString = 0;
	};
	
	void addMessageString( string msg ){
		messageStrings.push_back(msg);
	}
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	void loadImage( string image ){
		ofImage img;
		img.loadImage(image);
		images.push_back(img);
	}
	
	void update(){
		for (int i=particles.size()-1; i>=0; i--){
			particles[i]->update();
			if (!particles[i]->alive()) particles.erase(particles.begin()+i);
		}
	};
	
	void emit( int index = 0){
		if (ofGetElapsedTimeMillis() - lastEmitted > EMITTER_TIME){
			cout <<"emit!"<<endl;
			Particle * part = new Particle();
			part->setLoc(0, ofGetHeight());
			if (index > images.size()-1 || lastFoundString < 0) index = 0;
			part->setImage(&images[index]);
			particles.push_back(part);
			lastEmitted = ofGetElapsedTimeMillis();
		}
		cout <<"too soon!"<<endl;
	}
	
	void draw(){
		ofPushMatrix();{
			ofTranslate(loc.x, loc.y);
			for (int i=0; i<particles.size(); i++){
				particles[i]->draw();
			}
		} ofPopMatrix();
	};
	
	bool checkMessageString(string msg){
		for (int i=0; i<messageStrings.size(); i++){
			if (messageStrings[i] == msg){
				lastFoundString = i;
				return true;
			}
		}
		return false;
	}
	
	vector<string> messageStrings;
	int lastFoundString;
	
private:
	vector<ofImage> images;
	vector <Particle *> particles;
	int lastEmitted;
	ofPoint loc;
};
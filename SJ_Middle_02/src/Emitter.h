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
#include "ofEvents.h"
#include "Particle.h"

#define EMITTER_TIME 500

class ParticleEventArgs : public ofEventArgs {
	public:
		ofPoint loc;
		string name;
		string address;
};

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
	
	void setName (string _name){
		name = _name;
	}
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	void loadImage( string image ){
		ofImage * img = new ofImage();
		img->loadImage(image);
		images.push_back(img);
	}
	
	void update(){
		static ParticleEventArgs particleArgs;
		for (int i=particles.size()-1; i>=0; i--){
			particles[i].update();
			if (!particles[i].alive()){
				particleArgs.loc.x = loc.x;
				particleArgs.loc.y = particles[i].loc.y;
				particleArgs.name = name;				
				particleArgs.address = messageStrings[0];
				ofNotifyEvent(particleLeft, particleArgs, this);
				//Particle toDelete = particles[i];
				particles.erase(particles.begin()+i);
				//delete toDelete;
			}
		}
	};
	
	void emit( int index = 0){
		if (ofGetElapsedTimeMillis() - lastEmitted > EMITTER_TIME){
			cout <<"emit!"<<endl;
			Particle part;
			part.setLoc(0, ofGetHeight());
			if (index > images.size()-1 || lastFoundString < 0) index = 0;
			part.setImage(images[index]);
			part.setScale(.25);
			particles.push_back(part);
			lastEmitted = ofGetElapsedTimeMillis();
		}
		cout <<"too soon!"<<endl;
	}
	
	void draw(){
		ofPushMatrix();{
			ofTranslate(loc.x, loc.y);
			for (int i=0; i<particles.size(); i++){
				particles[i].draw();
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
	
	ofEvent<ParticleEventArgs> particleLeft;
	
private:
	vector<ofImage *> images;
	vector <Particle> particles;
	int lastEmitted;
	ofPoint loc;
	string name;
};
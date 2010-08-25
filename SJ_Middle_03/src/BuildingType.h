/*
 *  BuildingType.h
 *  SJ Stack 02
 *
 *  Created by brenfer on 7/8/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 
 THIS STORES ALL THE STUFF RELATED TO A TYPE:
 - name
 - messageStrings (for OSC)
 - 3D Model(s) [associated to messagestrings] 
 */

#pragma once

#include "ofx3DModelLoader.h"

class BuildingType
{
public:	
	
	BuildingType(){		
		lastEmitted = ofGetElapsedTimeMillis();
		EMITTER_TIME = 500;
	}
	
	void setEmitterTime( int time ){
		EMITTER_TIME = time;
	} 
	
	int getEmitterTime(){
		return EMITTER_TIME;
	}
		
	bool canEmit(){
		
		if (ofGetElapsedTimeMillis() - lastEmitted > EMITTER_TIME){
			lastEmitted = ofGetElapsedTimeMillis();
			return true;
		}
		return false;
	}
	
	void setName (string _name){
		name = _name;
	}
	
	string getName (){
		return name;
	}
	
	void setPosition( float x, float y){
		emitPosition.x = x;
		emitPosition.y = y;
	}
	
	ofPoint getPosition(){
		return emitPosition;
	}
	
	string getRandomMessageString(){
		return messageStrings[(int)ofRandom(0, messageStrings.size())];
	}
	
	int getRandomMessageStringIndex(){
		return (int)ofRandom(0, messageStrings.size());
	}
		
	
	void addMessageString( string msg ){
		messageStrings.push_back(msg);
	};
	
	void loadImage( string image ){
		ofImage * img = new ofImage();
		images.push_back(img);
	};
	
	string getMessageString (int index ){
		return messageStrings[index];
	};
	
	ofx3DModelLoader * getModel( int index ){
		if (index > models.size()) index = models.size()-1;
		if (index < 0) index = 0;
		if (models.size() < 1) return new ofx3DModelLoader();
		//return models[index];
		return models[(int) ofRandom(0, models.size())];

	};
	
	void loadModel( string _model, float scale=1.0f ){
		ofx3DModelLoader * model = new ofx3DModelLoader();
		model->loadModel(_model, 1.0);
		model->setScale(scale, scale, scale);
		//model->setPosition(model->getXdim()/2.0, model->getYdim()/2.0f, model->getZdim()/2.0f);
		models.push_back(model);
	};
	
	int checkMessageString(string msg){
		for (int i=0; i<messageStrings.size(); i++){
			if (messageStrings[i] == msg){
				lastFoundString = i;
				return lastFoundString;
			}
		}
		return -1;
	};
	
private:	
	
	vector<string> messageStrings;
	vector<ofImage *> images;
	vector<ofx3DModelLoader *> models;
	
	int lastFoundString;
	int lastEmitted;
	string name;
	
	int EMITTER_TIME;
	
	ofPoint emitPosition;
};
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
//		modelImageListIndex = -1;
//		model = new ofx3DModelLoader();
		
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
	
	void loadModel( string modelImage ){
		modelImageList.push_back(modelImage);
		ofx3DModelLoader* partModel = new ofx3DModelLoader();
		partModel->loadModel(modelImage,1);
		models.push_back(partModel);
	}
	
	//Memory management
	void update(){
		static ParticleEventArgs particleArgs;
		for (int i=particles.size()-1; i>=0; i--){
			particles[i]->update();
			
			if (!particles[i]->alive()){
				particleArgs.loc.x = loc.x + particles[i]->loc.x; // must add because we translate to each emitter's x-coordinate with particle coordinate initially = 0.
				particleArgs.loc.y = particles[i]->loc.y;
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
			if (index > models.size()-1 || lastFoundString < 0) index = 0;
			
			Particle* part = new Particle();
			part->setLoc(0, ofGetHeight());			
			part->setModel(models[index]);
			part->setScale(2);
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
	
	void emitRandom( ){
		int ran = (int) ofRandom(0, messageStrings.size());				
		emit(ran);
	};
	
	vector<string> messageStrings;
	int lastFoundString;
	
	ofEvent<ParticleEventArgs> particleLeft;
	
	string getName() {
		return name;
	}
	
private:
//	vector<ofImage *> images;
	vector<string> modelImageList;
//	int modelImageListIndex;
	vector<ofx3DModelLoader* > models;
//	ofx3DModelLoader* model;
	vector <Particle* > particles;
	int lastEmitted;
	ofPoint loc;
	string name;
};
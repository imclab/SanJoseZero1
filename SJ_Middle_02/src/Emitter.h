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
//		ofImage * img = new ofImage();
//		img->loadImage(image);
//		images.push_back(img);
		
//		ofx3DModelLoader* mdl = new ofx3DModelLoader();
//		mdl->loadModel(model,10);
//		models.push_back(mdl);

//		model->loadModel(model,10);

		modelImageList.push_back(modelImage);
//		modelImageListIndex++;
		
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
			Particle* part = new Particle();
			ofx3DModelLoader* partModel = new ofx3DModelLoader();
// MUST FIX THIS IF GOING TO HAVE MORE THAN ONE IMAGE, WHICH WE WILL!			
			partModel->loadModel(modelImageList[0],1);
			part->setLoc(0, ofGetHeight());
			if (index > models.size()-1 || lastFoundString < 0) index = 0;
//			part->setImage(models[index]);
			part->setModel(partModel);
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
	
	vector<string> messageStrings;
	int lastFoundString;
	
	ofEvent<ParticleEventArgs> particleLeft;
	
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
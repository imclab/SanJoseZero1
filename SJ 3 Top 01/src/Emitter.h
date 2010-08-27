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
#include "ofx3DModelLoader.h"

#define EMITTER_TIME 500
#define ROTATE_TIME 5000

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
		ceiling = originalCeiling = 400;
		lastRotated = ofGetElapsedTimeMillis();
		rotateAmount = 0;
		baseLoc = 0;
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
		//img.loadImage(image);
		images.push_back(img);
	}
	
	void loadModel( string _model, float scale ){
		ofx3DModelLoader * model = new ofx3DModelLoader();
		model->loadModel(_model, 1.0);
		model->setScale(scale, scale, scale);
		//img.loadImage(image);
		models.push_back(model);
	}
	
	void update(){
		if (ofGetElapsedTimeMillis() - lastRotated > ROTATE_TIME){
			rotateAmount = 30;
			ceiling = originalCeiling;
			lastRotated = ofGetElapsedTimeMillis();
			vector <Particle> realCurrent;
			for (int i=0; i<currentStack.size(); i++){
				realCurrent.push_back(currentStack[i]);
			}
			if (currentStack.size() > 0) 
				baseLoc += ofRandom(-50, 50);
			currentStack.clear();
			deadParticles.push_back(realCurrent);
		}
		
		for (int j=0; j<particles.size(); j++){
			particles[j].setCeiling(ceiling);
		}
		
		if (rotateAmount > 0){
			for (int i=deadParticles.size()-1; i>=0; i--){
				
				for (int j=0; j<deadParticles[i].size(); j++){
					if (j != 0){
						deadParticles[i][j].loc.y = deadParticles[i][0].loc.y;
						deadParticles[i][j].index = j;
					}
					if (deadParticles[i][j].rotate.x < 95){
						deadParticles[i][j].rotate.x += 1;
						deadParticles[i][j].rotate.z += 5.0/90.0f;
						deadParticles[i][j].loc.y -= .5;
					} else {
						deadParticles[i][j].loc.y -= 1;
					}
				}
				//off screen
				if (deadParticles[i][deadParticles[i].size()-1].loc.y < -100){
					deadParticles.erase(deadParticles.begin()+i);
				}
			}
			rotateAmount -= 1;
		}
		
		for (int i=particles.size()-1; i>=0; i--){
			bool wasAlive = particles[i].alive();
			particles[i].update();
			if (wasAlive && !particles[i].alive()){			
				ceiling = particles[i].loc.y + particles[i].getWidth();
				currentStack.push_back(particles[i]);
				particles.erase(particles.begin()+i);
				
				for (int j=0; j<particles.size(); j++){
					particles[j].setCeiling(ceiling);
				}
			}
		}
	};
	
	void emit( int index = 0){
		if (ofGetElapsedTimeMillis() - lastEmitted > EMITTER_TIME){
			cout <<"emit!"<<endl;
			Particle part;
			part.setLoc(baseLoc, ofGetHeight());
			part.setCeiling(ceiling);
			if (index > models.size()-1 || lastFoundString < 0) index = 0;
			//part.setImage(images[index]);
			part.setImage(models[index]);
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
			for (int i=0; i<currentStack.size(); i++){
				currentStack[i].draw();
			}
			for (int i=0; i<deadParticles.size(); i++){
				for (int j=0; j<deadParticles[i].size(); j++){
					deadParticles[i][j].draw();
				}
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
	float originalCeiling;
	
private:
	vector<ofImage *> images;
	vector<ofx3DModelLoader *> models;
	vector <Particle> particles;
	vector <Particle> currentStack;
	vector <vector <Particle> > deadParticles;
	int lastEmitted;
	ofPoint loc;
	string name;
	float ceiling;
	float baseLoc;
	
	float rotateAmount;
	int lastRotated;
};
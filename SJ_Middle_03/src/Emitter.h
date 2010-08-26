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
#include "BuildingType.h"
#include "Particle.h"
#include "Columns.h"

#define EMITTER_TIME 500

class ParticleEventArgs : public ofEventArgs {
	public:
		ofPoint loc;
		//string name;
		string address;
		string data;
};

class Emitter
{
public:
	Emitter(){
		lastEmitted = ofGetElapsedTimeMillis();	
	};
	
	void addType( BuildingType * type ){
		types.push_back(type);
	};
	
	int getNumTypes(){
		return types.size();
	}
	
	void attachColumns( Columns * _columns){
		columns = _columns;
	};
		
	//Memory management
	void update(){
		
		//first, update all the particles emitted with their neighbors
		ofPoint endPoint;
		
		//figure out how far into negative we need to go
		float negativeStackHeight = 0;
		
		for (int i=0; i<currentParticles.size(); i++){
			negativeStackHeight += currentParticles[i]->getHeight();
		}
		
		for (int i=0; i<currentParticles.size(); i++){
			if (i == 0){
				endPoint = currentParticles[i]->getEndPoint();
				endPoint.y = -negativeStackHeight;
				//endPoint.z += ofRandom(-200,400);
			}
			currentParticles[i]->setIndex(i);
			for (int j=0; j<currentParticles.size(); j++){
				if (j != i){
					currentParticles[i]->addNeighbor(currentParticles[j]);
					//adjust its end position
					currentParticles[i]->setEndPoint(endPoint.x, endPoint.y, endPoint.z);
					endPoint.y += currentParticles[i]->getHeight();
				}
			};
		};
		
		// then clear out the array
		currentParticles.clear();
		
		//then do the normal update routine
		
		static ParticleEventArgs particleArgs;
		for (int i=particles.size()-1; i>=0; i--){
			particles[i]->update();
			
			if (!particles[i]->alive()){
				particleArgs.loc.x = particles[i]->getLoc().x;
				particleArgs.loc.y = particles[i]->getLoc().y;
				particleArgs.address = particles[i]->getMessageString();
				particleArgs.data = particles[i]->getData();
				
				ofNotifyEvent(particleLeft, particleArgs, this);
				//Particle toDelete = particles[i];
				particles.erase(particles.begin()+i);
				//delete toDelete;
			}
		}
	};
	
	void emit( int whichType, int index = 0, string data = ""){
		BuildingType * type = types[whichType];
		if (type->canEmit()){			
			Particle* part = new Particle();
			part->setLoc(type->getPosition().x, ofGetHeight());			
			part->setModel(type->getModel(index));
			part->setEndPoint(columns->getClosestColumn(type->getPosition().x).x, columns->getClosestColumn(type->getPosition().x).y);
			part->setMessageString(types[whichType]->getMessageString(index));
			part->setData(data);
			
			particles.push_back(part);
			currentParticles.push_back(part);
			lastEmitted = ofGetElapsedTimeMillis();
		}
		cout <<"too soon!"<<endl;
	}
	
	void draw(){
		ofPushMatrix();{
			//ofTranslate(loc.x, loc.y);
			for (int i=0; i<particles.size(); i++){
				particles[i]->draw();
			}
		} ofPopMatrix();
	};
	
	bool checkMessageString(string msg){
		for (int i=0; i<types.size(); i++){
			if (types[i]->checkMessageString(msg) >= 0)
				return true;
		}
		return false;
	}
	
	bool checkAndEmit(string msg, string data=""){		
		for (int i=0; i<types.size(); i++){
			int index = types[i]->checkMessageString(msg);
			if (index >= 0){
				emit(i, index, data);
				return true;
			}
		}
		return false;
	};
	
	void emitRandom(int which){
		int index = types[which]->getRandomMessageStringIndex();
		emit(which, index);
	};
	
	void debug( int x, int y){
		if (particles.size() > 0)
			particles[0]->setLoc(x,y);
	}
	ofEvent<ParticleEventArgs> particleLeft;
	
	vector <Particle* > particles;
	
private:
	int lastEmitted;
	Columns * columns;
	vector <BuildingType *> types;
	
	//running vector of particles emitted on this frame
	// NOTE: CHANGE THIS TO BE MORE FLEXIBLE (e.g. not emitted on the EXACT same frame...)
	
	vector <Particle *> currentParticles;
};
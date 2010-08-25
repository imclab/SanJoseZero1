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
			part->setScale(2);
			part->setEndPoint(columns->getClosestColumn(type->getPosition().x).x, columns->getClosestColumn(type->getPosition().x).y);
			part->setMessageString(types[whichType]->getMessageString(index));
			part->setData(data);
			part->start();
			
			particles.push_back(part);
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
	
	ofEvent<ParticleEventArgs> particleLeft;
	
private:
	vector <Particle* > particles;
	int lastEmitted;
	Columns * columns;
	vector <BuildingType *> types;
};
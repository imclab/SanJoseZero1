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
#include "ofxFBOTexture.h"

#define NUM_DRAW_TYPES 2

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
	
/***************************************************************
	CONSTRUCTOR
***************************************************************/
	
	Emitter(){
		lastEmitted = lastGrouped = ofGetElapsedTimeMillis();	
		particleGroupTolerance = 300;
		
		//calibration stuff
		bStartDragging = bEndDragging = false;
		setTransformStart( ofGetHeight() *3./4.);
		setTransformEnd( ofGetHeight() / 4. );		
		setMinScale(3.0f);
		setMaxScale(10.0f);
	};

/***************************************************************
	 UPDATE: GROUP PARTICLES, UPDATE POSITIONS, CLEAR DEAD BROS
***************************************************************/
	
	void update(){
		
		//first, update all the particles emitted with their neighbors
		ofPoint endPoint;
		
		//figure out how far into negative we need to go
		float negativeStackHeight = 0;
		
		for (int i=0; i<currentParticles.size(); i++){
			negativeStackHeight += currentParticles[i]->getHeight();
			currentParticles[i]->clearNeighbors();
		}
		
		for (int i=0; i<currentParticles.size(); i++){
			if (i == 0){
				negativeStackHeight += currentParticles[i]->getHeight();
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
					//endPoint.y = -currentParticles[i]->getHeight();
				}
			};
		};
		
		// then clear out the array
		if (ofGetElapsedTimeMillis() - lastGrouped > particleGroupTolerance){
			lastGrouped = ofGetElapsedTimeMillis();
			currentParticles.clear();
		}
		//then do the normal update routine
		
		static ParticleEventArgs particleArgs;
		for (int i=0; i<particles.size(); i++){
			particles[i]->update();
			
			//find closest column again to limit more intense scale out
			if (particles[i]->bLeaving && particles[i]->isMasterParticle() && !particles[i]->bFoundNewColumn){	
				particles[i]->bFoundNewColumn = true;
				particles[i]->setEndPointX(columns->getClosestColumn(particles[i]->getLoc().x).x);
				particles[i]->setTargetPointX(columns->getClosestColumn(particles[i]->getLoc().x).x);
				// you need to reset both the endpoint AND the target so it clicks into place then drifts offscreen
			}
			
		}
		for (int i=particles.size()-1; i>=0; i--){			
			if (particles[i]->send() && !particles[i]->bSent){
				particleArgs.loc.x = particles[i]->getLoc().x;
				particleArgs.loc.y = particles[i]->getLoc().y;
				particleArgs.address = particles[i]->getMessageString();
				particleArgs.data = particles[i]->getData();
				ofNotifyEvent(particleLeft, particleArgs, this);
				particles[i]->bSent = true;
			} else if (!particles[i]->alive()){
				if (!particles[i]->bSent){
					particleArgs.loc.x = particles[i]->getLoc().x;
					particleArgs.loc.y = particles[i]->getLoc().y;
					particleArgs.address = particles[i]->getMessageString();
					particleArgs.data = particles[i]->getData();
					ofNotifyEvent(particleLeft, particleArgs, this);
				}
				if (particles[i]->okToErase()){
					particles.erase(particles.begin()+i);
				}
			}
		}
	};

	void draw(){
		ofEnableAlphaBlending();
		ofPushMatrix();{
						
			for (int i=0; i<particles.size(); i++){
				particles[i]->draw();
			}
		} ofPopMatrix();
		ofDisableAlphaBlending();
	};
	
/***************************************************************
	 EMIT
***************************************************************/
	
	void emit( int whichType, int index = 0, string data = ""){
		BuildingType * type = types[whichType];
		if (type->canEmit()){			
			Particle* part = new Particle();
			part->setLoc(type->getPosition().x, type->getPosition().y);		
			part->setTransformStart(transformStart);
			part->setTransformEnd(transformEnd);
			part->setMinScale(minScale);
			part->setMaxScale(maxScale);
			
			part->setModel(type->getModel(index));
			part->setType(type);
			part->setEndPoint(columns->getClosestColumn(type->getPosition().x).x, columns->getClosestColumn(type->getPosition().x).y);
			part->setMessageString(type->getMessageString(index));
			part->setData(data);
			part->drawType = (int) ofRandom(0, NUM_DRAW_TYPES);
			part->setColor(type->getColor(index));
			
			particles.push_back(part);
			currentParticles.push_back(part);
			lastEmitted = ofGetElapsedTimeMillis();
		}
	}
	
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
	
/***************************************************************
	GET PARTICLES
 ***************************************************************/	
	
	vector <Particle* > particles;
	
/***************************************************************
	 GET + SET CALIBRATION STUFF
***************************************************************/	
	
	bool bStartDragging, bEndDragging;
	
	void addType( BuildingType * type ){
		types.push_back(type);
	};
	
	int getNumTypes(){
		return types.size();
	}
	
	BuildingType * getType( int which ){
		return types[which];
	}
	
	float getTransformStart(){
		return transformStart;
	};
	
	void setTransformStart( float _start ){
		transformStart = _start;
		for (int i=0; i<particles.size(); i++){
			particles[i]->setTransformStart(transformStart);
		}
	}
	
	float getTransformEnd(){
		return transformEnd;
	};
	
	void setTransformEnd( float _end ){
		transformEnd = _end;		
		for (int i=0; i<particles.size(); i++){
			particles[i]->setTransformEnd(transformEnd);
		}
	};	
		
	void setMinScale( float _minscale ){
		minScale = _minscale;
		
		for (int i=0; i<particles.size(); i++){
			particles[i]->setMinScale(minScale);
		};		
	};
	
	void setMaxScale( float _maxscale ){
		maxScale = _maxscale;
		
		for (int i=0; i<particles.size(); i++){
			particles[i]->setMaxScale(maxScale);
		};		
	};
	
	void setGroupingTolerance( int _grouping ){
		particleGroupTolerance = _grouping;
	};

/***************************************************************
	 TRANSFORM DEBUG
***************************************************************/	
	
	void drawTransformDebug(){
		if (bStartDragging)
			ofSetColor(0xff0000);
		else 
			ofSetColor(0xffffff);
		ofDrawBitmapString("transform start", 10, transformStart - 10);
		ofLine(0, transformStart, ofGetWidth(), transformStart);
		
		if (bEndDragging)
			ofSetColor(0xff0000);
		else 
			ofSetColor(0xffffff);
		ofDrawBitmapString("transform end", 10, transformEnd - 10);
		ofLine(0, transformEnd, ofGetWidth(), transformEnd);		
	};
	
/***************************************************************
	 GET + SET CALIBRATION STUFF
***************************************************************/	
	
	void mousePressedTransform( int x, int y ){
		if (fabs(y - transformStart) < 5){
			bStartDragging = true;
		} else if (fabs(y - transformEnd) < 5){
			bEndDragging = true;
		} else {
			for (int i=0; i<types.size(); i++){
				types[i]->pressed(x,y);
			};
		};
	};

/***************************************************************
	 MOUSE + WINDOW UTILS
***************************************************************/	
	
	void mouseDragged( int x, int y ){
		if (bStartDragging){
			if (y > transformEnd + 5)
				transformStart = y;
			else 
				transformStart = transformEnd + 5;
		} else if (bEndDragging){
			if (y < transformStart - 5)
				transformEnd = y;
			else 
				transformEnd = transformStart - 5;
		} else {
			for (int i=0; i<types.size(); i++){
				if (types[i]->bPressed){
					types[i]->setPosition(x,y);
				}
			}
		};
	};
	
	void mouseReleased(){
		bStartDragging = bEndDragging = false;
		for (int i=0; i<types.size(); i++){
			types[i]->bPressed = false;
		}
		//trailsFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	};
	
	
/***************************************************************
	 DRAW DEBUG 
***************************************************************/	
	
	void drawTypes(){
		for (int i=0; i<types.size(); i++){
			ofSetColor(150,0,0);
			ofRect(types[i]->getPosition().x, types[i]->getPosition().y, 20, 20);
			ofSetColor(0xffffff);
			ofDrawBitmapString(types[i]->getName(), types[i]->getPosition().x, types[i]->getPosition().y-30);
		};
	};
	
	void drawDebugParticles(){
		if (types.size() > 0){
					
			//little guy
			types[0]->drawDebug(ofGetWidth()/2.0 - types[0]->getWidth(minScale)*2., ofGetHeight()/2.0f, minScale);
			
			//big guy
			types[0]->drawDebug(ofGetWidth()/2.0 + types[0]->getWidth(maxScale)*2., ofGetHeight()/2.0f, maxScale);
		}
	};
	
/***************************************************************
	GET + SET CALIBRATION STUFF
***************************************************************/	
	
	void attachColumns( Columns * _columns){
		columns = _columns;
	};
	
	ofEvent<ParticleEventArgs> particleLeft;
	
private:
	int lastEmitted;
	Columns * columns;
	vector <BuildingType *> types;	
	
	//running vector of particles emitted on this frame
	// NOTE: CHANGE THIS TO BE MORE FLEXIBLE (e.g. not emitted on the EXACT same frame...)
	
	int particleGroupTolerance;
	int lastGrouped;
	vector <Particle *> currentParticles;
	
	//calibration
	float transformStart;
	float transformEnd;
	float minScale, maxScale;
	
};
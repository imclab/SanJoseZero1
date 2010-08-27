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
		lastEmitted = lastGrouped = ofGetElapsedTimeMillis();	
		particleGroupTolerance = 300;
		
		//calibration stuff
		bStartDragging = bEndDragging = false;
		setTransformStart( ofGetHeight() *3./4.);
		setTransformEnd( ofGetHeight() / 4. );		
		//trailsFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
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
					cout<<endPoint.y<<endl;
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
				particles.erase(particles.begin()+i);
			}
		}
	};
	
	void emit( int whichType, int index = 0, string data = ""){
		BuildingType * type = types[whichType];
		if (type->canEmit()){			
			Particle* part = new Particle();
			part->setLoc(type->getPosition().x, type->getPosition().y);			
			part->setModel(type->getModel(index));
			part->setEndPoint(columns->getClosestColumn(type->getPosition().x).x, columns->getClosestColumn(type->getPosition().x).y);
			part->setMessageString(types[whichType]->getMessageString(index));
			part->setData(data);
			part->setTransformStart(transformStart);
			part->setTransformEnd(transformEnd);
			
			particles.push_back(part);
			currentParticles.push_back(part);
			lastEmitted = ofGetElapsedTimeMillis();
		}
		cout <<"too soon!"<<endl;
	}
	
	void draw(){
		ofEnableAlphaBlending();
		ofPushMatrix();{
			
			ofSetColor(0xffffff);
			/*glDisable(GL_DEPTH_TEST);
			trailsFBO.draw(0, 0);
			glEnable(GL_DEPTH_TEST);*/
			
			//ofTranslate(loc.x, loc.y);
			for (int i=0; i<particles.size(); i++){
				/*if (particles[i]->bMagnifying){
					trailsFBO.swapIn();
					glDisable(GL_DEPTH_TEST);
					particles[i]->draw();
					glEnable(GL_DEPTH_TEST);
					trailsFBO.swapOut();
				}*/
				particles[i]->draw();
			}
			/*trailsFBO.begin();
			ofSetColor(0,0,0,10);
			glDisable(GL_DEPTH_TEST);
			ofRect(0,0,trailsFBO.getWidth(), trailsFBO.getHeight());
			trailsFBO.end();*/
		} ofPopMatrix();
		ofDisableAlphaBlending();
	};
	
/***************************************************************
	 EMIT
***************************************************************/	
	
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
		//trailsFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
	};
	
	void windowResized( int x, int y){
		//trailsFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
	}
	
	void drawTypes(){
		for (int i=0; i<types.size(); i++){
			ofSetColor(150,0,0);
			ofRect(types[i]->getPosition().x, types[i]->getPosition().y, 20, 20);
			ofSetColor(0xffffff);
			ofDrawBitmapString(types[i]->getName(), types[i]->getPosition().x, types[i]->getPosition().y-30);
		};
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
	
	ofxFBOTexture trailsFBO;
};
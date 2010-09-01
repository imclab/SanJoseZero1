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
		setEmitterTime(0);
		bPressed = false;
	}
	
/***************************************************************
	 GETTERS + SETTERS
***************************************************************/
	
	//emit time
	
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
	
	//name
	
	void setName (string _name){
		name = _name;
	}
	
	string getName (){
		return name;
	}
	
	//position
	
	void setPosition( float x, float y){
		emitPosition.x = x;
		emitPosition.y = y;
	}
	
	ofPoint getPosition(){
		return emitPosition;
	}
	
	//message strings
	
	string getRandomMessageString(){
		return messageStrings[(int)ofRandom(0, messageStrings.size())];
	}
	
	int getRandomMessageStringIndex(){
		return (int)ofRandom(0, messageStrings.size());
	}
		
	
	void addMessageString( string msg ){
		messageStrings.push_back(msg);
	};
	
	
	string getMessageString (int index ){
		return messageStrings[index];
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
	
	// models
	
	ofx3DModelLoader * getModel( int index ){
		if (index > models.size()) index = models.size()-1;
		if (index < 0) index = 0;
		if (models.size() < 1) return new ofx3DModelLoader();
		//return models[index];
		return models[(int) ofRandom(0, models.size())];

	};
		
	float getHeight( float scale = 1.0 ){
		if (models.size() > 0){
			models[0]->setScale(1.0f, 1.0f, 1.0f);
			return models[0]->getYdim()*scale;
		} else {
			return 0.;
		}
	}
	
	float getWidth( float scale = 1.0 ){
		if (models.size() > 0){
			models[0]->setScale(1.0f, 1.0f, 1.0f);
			return models[0]->getXdim()*scale;
		} else {
			return 0.;
		}
	}
	
	void loadImage( string image ){
		ofImage * img = new ofImage();
		images.push_back(img);
	};
	
	void loadModel( string _model, float scale=1.0f ){
		ofx3DModelLoader * model = new ofx3DModelLoader();
		model->loadModel(_model, 1.0);
		model->setScale(scale, scale, scale);
		//model->setPosition(model->getXdim()/2.0, model->getYdim()/2.0f, model->getZdim()/2.0f);
		models.push_back(model);
		modelScales.push_back(scale);
	};
	
/***************************************************************
	MOUSE 
***************************************************************/
	
	bool bPressed;
	
	bool pressed (int mx, int my){
		int tolerance = 20;
		if ( mx >= emitPosition.x && mx <= emitPosition.x + tolerance 
			&& my >= emitPosition.y && my <= emitPosition.y + tolerance){
			bPressed = true;
			return true;
		};
		bPressed = false;
		return false;
	};
	
/***************************************************************
	DEBUG 
***************************************************************/
	
	void drawDebug( int x, int y ){
		if (models.size() > 0){
			ofPushMatrix();{
				models[0]->setScale(modelScales[0],modelScales[0],modelScales[0]);
				ofTranslate(x, y);
				models[0]->draw();
			} ofPopMatrix();
		};
	};
	
	void drawDebug( int x, int y, float scale ){
		if (models.size() > 0){
			ofPushMatrix();{
				models[0]->setScale(scale,scale,scale);
				ofTranslate(x, y);
				models[0]->draw();
			} ofPopMatrix();
		};
	};
	
private:	
	
	vector<string> messageStrings;
	vector<ofImage *> images;
	vector<ofx3DModelLoader *> models;
	vector<float> modelScales;
	
	int lastFoundString;
	int lastEmitted;
	string name;
	
	int EMITTER_TIME;
	
	
	ofPoint emitPosition;
};
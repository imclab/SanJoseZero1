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
#include "ofx3DModelLoader.h"
#include "ofxXmlSettings.h"
#include "BuildingRow.h"
#include "BuildingType.h"
#include "Globals.h"

class Emitter
{
public:
	
	Emitter( float _ceiling ){
		ceiling = _ceiling;
		rotateAmount = 0;
		lastRotated = 0;//ofGetElapsedTimeMillis();
		
		//setup different building types
		
		ofxXmlSettings settings;
		settings.loadFile("settings.xml");
		settings.pushTag("settings");
		
		int numReceivers = settings.getNumTags("input");
		for (int i=0; i<numReceivers; i++) {
			
			BuildingType * b = new BuildingType();
									
			if (i <numReceivers){			
				settings.pushTag("input", i);
				b->setName(settings.getValue("name", "") );
				
				for (int j=0; j<settings.getNumTags("message"); j++){
					settings.pushTag("message", j);
						b->addMessageString(settings.getValue("messageString", ""));
						for (int k=0; k<settings.getNumTags("image"); k++){
							b->loadModel(ofToDataPath(settings.getValue("image", "", k)), 5.0);
						}
					settings.popTag();
				}
				settings.popTag();
			};
			
			types.push_back(b);
		}
		settings.popTag();
		
		//set up first row		
		currentRow = new BuildingRow(ceiling);
		cout<<"emitter set up"<<endl;
	};

	
/***********************************************************
	CEILING!
***********************************************************/
	
	float ceiling;
	
	void setCeiling( float _ceiling){
		ceiling = _ceiling;
		currentRow->setBasePosition(ceiling);
	};
	
/***********************************************************
	UPDATE + DRAW
***********************************************************/
	
	void update(){
		//1 - see if it's time to rotate the rows
		if (ofGetElapsedTimeMillis() - lastRotated > ROTATE_TIME){
			
			rotateAmount = ROTATE_AMOUNT;			
			lastRotated = ofGetElapsedTimeMillis();
			newRow();
		}	
		
		//2 - rotate
		if (rotateAmount > 0){
			*advanceCeiling = true;
			rotateAmount -= ROTATE_INCREMENT;
		}	
		else {
			//LARS this points to a bool in the testApp that moves conveyor
			//by advancing U position in the pointOnCurveNodes
			*advanceCeiling = false;
		}
		
		//3 - update current row
		currentRow->update();
		
		//4 - update ceiling
		for (int i=0; i<buildings.size(); i++){
			buildings[i]->setCeiling(currentRow->getCeiling(buildings[i]->getWhichStack()));
		}
		
		//5 - update moving particles
		for (int i=buildings.size()-1; i>=0; i--){
			bool bWasAlive = buildings[i]->alive();
			buildings[i]->update();
			
			//hit ceiling! add to stack, remove from current vector + update ceilings of all particles
			if (bWasAlive && !buildings[i]->alive()){	
				currentRow->addToStack( buildings[i] );
				buildings.erase(buildings.begin()+i);
				
				for (int j=0; j<buildings.size(); j++){
					buildings[j]->setCeiling( currentRow->getCeiling( buildings[j]->getWhichStack() ) );
				}
			}
		}
	};
	
	void draw(){
		glDisable(GL_DEPTH_TEST);
		currentRow->draw();		
		glEnable(GL_DEPTH_TEST);
		
		for (int i=0; i<buildings.size(); i++){
			buildings[i]->draw();
		}
	};	
		
	void newRow(){
		currentRow->setComplete(true);
		rowIsComplete(currentRow);
		delete currentRow;
		currentRow = new BuildingRow(ceiling);
	};
	
	void emitRandom( ){
		int ran = (int) ofRandom(0, types.size());
		
		float ranX = ofRandom(0, ofGetWidth());
			
		checkMessageString(types[ran]->getRandomMessageString(), ranX);
	};
	
	void setAdvanceCeilingBool(bool *pointer2advCeilBool){advanceCeiling = pointer2advCeilBool;}
/***********************************************************
	MANAGE ASSETS
***********************************************************/
		
	bool checkMessageString(string msg, float position, float speed=-5.0f, string data=""){
		for (int i=0; i<types.size(); i++){
			int messageIndex = types[i]->checkMessageString(msg);
			if (messageIndex >= 0){		
				
				//try to emit
				if (types[i]->emit()){
					Building * part = new Building();
					Stack * stack = currentRow->getClosestStack(position);
					part->setStackIndex(stack->index);
					part->setCeiling( currentRow->getCeiling(stack->index) );					
					part->setImage( types[i]->getModel(messageIndex) );		
					part->setPos( stack->getPosition().x, types[i]->getPosition().y + part->getHeight(), 0 );		
					part->setType( types[i]->getMessage(messageIndex) );
					part->speed = speed;
					part->setData( data);
					buildings.push_back(part);
				}				
				lastFoundString = i;
				return true;
			}
		}
		return false;
	}
	
	int lastFoundString;
	float originalCeiling;
	
	void windowResized(){
		if ( currentRow != NULL)
			currentRow->windowResized();
		
		for (int i=0; i<types.size(); i++){			
			types[i]->setPosition( 25 + (float)(ofGetWidth()/types.size())*i, ofGetHeight() );
		}
	}
	
/***********************************************************
	LOGGING
***********************************************************/
	
	ofEvent<BuildingRow*> rowComplete;
	
	void rowIsComplete( BuildingRow * completedRow ){
    	ofNotifyEvent(rowComplete, completedRow, this);			
	};
	
private:	
	vector <Building *> buildings;
	BuildingRow * currentRow;
	
	vector <BuildingType *> types;	
	float rotateAmount;
	int lastRotated;
public:
	bool *advanceCeiling;
};
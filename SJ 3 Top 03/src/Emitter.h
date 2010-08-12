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
	
	Emitter(){
		//lastRotated = ofGetElapsedTimeMillis();
		
		//setup different building types
		
		ofxXmlSettings settings;
		settings.loadFile("settings.xml");
		settings.pushTag("settings");
		
		int numReceivers = settings.getNumTags("catcher");
		for (int i=0; i<numReceivers; i++) {
			
			BuildingType * b = new BuildingType();
			
			//set base emitting positions
			b->setPosition( 25 + (float)(ofGetWidth()/numReceivers)*i, ofGetHeight() + 25 );
						
			if (i <numReceivers){			
				settings.pushTag("catcher", i);
				b->setName(settings.getValue("name", "") );
				
				for (int j=0; j<settings.getNumTags("message"); j++){
					settings.pushTag("message", j);
						b->addMessageString(settings.getValue("messageString", ""));
						for (int k=0; k<settings.getNumTags("image"); k++){
							b->loadModel(ofToDataPath(settings.getValue("image", "", k)), 5.0);
						}
						//cout << "loading "<<settings.getValue("image", "")<<":"<<settings.getValue("messageString", "")<<endl;
					settings.popTag();
				}
				settings.popTag();
			};
			
			types.push_back(b);
		}
		settings.popTag();
		
		//set up old rows
		int increment = 90/ROTATE_AMOUNT;
		
		float curY = 0;
		
		for (int i=1; i<=increment; i++){
			BuildingRow * dum = new BuildingRow(500);
			dum->rotation.x = 270 + ROTATE_AMOUNT*i;
			oldRows.push_back(dum);
			
			curY = dum->getSize().y;
		};
		//
//		for (int i=0; i<4; i++){
//			BuildingRow * dum = new BuildingRow(500);
//			dum->rotation.x = 360;
//			dum->pos.y += curY*(i+1);
//			oldRows.push_back(dum);
//			cout << dum->pos.y<<endl;
//			curY = 5;
//		}
				
		//fill up with 2 more
		
		BuildingRow * dum1 = new BuildingRow(500);
		dum1->rotation.x = 360;
		dum1->pos.y -= dum1->getSize().y-20;
		oldRows.push_back(dum1);
//		
//		BuildingRow * dum2 = new BuildingRow(500);
//		dum2->rotation.x = 360;
//		dum2->pos.y -= dum2->getSize().y-20;
//		dum2->pos.y -= dum2->getSize().y-20;
//		oldRows.push_back(dum2);
		
		//set up new row		
		currentRow = new BuildingRow(500);
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
			for (int i=oldRows.size()-1; i>=0; i--){
				if (oldRows[i]->rotation.x < 360){
					oldRows[i]->rotation.x += ROTATE_INCREMENT;
					//oldRows[i].rotate.z += 10.0/90.0f;
					//oldRows[i].pos.y -= deadBuildings[i][j].getWidth()/50.0f;
				} else {
					oldRows[i]->pos.y -= oldRows[i]->getSize().y/(ROTATE_AMOUNT/ROTATE_INCREMENT);
				}
				//off screen
				if (oldRows[i]->pos.y < -100){
					oldRows.erase(oldRows.begin()+i);
				}
			}
			rotateAmount -= ROTATE_INCREMENT;
		}
		
		//2 - update current row
		currentRow->update();
		
		//3 - update ceiling
		for (int i=0; i<buildings.size(); i++){
			buildings[i]->setCeiling(currentRow->getCeiling(buildings[i]->getWhichStack()));
		}
		
		//4 - update moving particles
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
		ofPushMatrix();{
			
			for (int i=0; i<oldRows.size(); i++){
				oldRows[i]->draw();
			}
			
			currentRow->draw();
						
			for (int i=0; i<buildings.size(); i++){
				buildings[i]->draw();
			}
		} ofPopMatrix();
	};	
	
	void newRow(){
		oldRows.push_back( currentRow );
		rowIsComplete(currentRow);
		currentRow = new BuildingRow(500);
	};
	
	void emitRandom( ){
		int ran = (int) ofRandom(0, types.size());
		
		checkMessageString(types[ran]->getRandomMessageString(), ofRandom(0, ofGetWidth()));
	};
	
/***********************************************************
	MANAGE ASSETS
***********************************************************/
		
	bool checkMessageString(string msg, float position){
		for (int i=0; i<types.size(); i++){
			int messageIndex = types[i]->checkMessageString(msg);
			if (messageIndex >= 0){		
				
				//try to emit
				if (types[i]->emit()){
					Building * part = new Building();
					Stack * stack = currentRow->getClosestStack(position);
					part->setStackIndex(stack->index);
					part->setPos( stack->getPosition().x , types[i]->getPosition().y, 0 );
					part->setCeiling( currentRow->getCeiling(stack->index) );					
					part->setImage( types[i]->getModel(messageIndex) );				
					part->setType( types[i]->getMessage(messageIndex) );
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
		for (int i=0; i<oldRows.size(); i++){
			oldRows[i]->windowResized();
		}
		
		currentRow->windowResized();
		
		for (int i=0; i<types.size(); i++){			
			types[i]->setPosition( 25 + (float)(ofGetWidth()/types.size())*i, ofGetHeight() + 25 );
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
	//vector <Building> particles;
	//vector <Building> currentStack;
	//vector <vector <Building> > deadBuildings;
	
	vector <Building *> buildings;
	BuildingRow * currentRow;
	vector<BuildingRow *> oldRows;	
	
	vector <BuildingType *> types;	
	
	float basepos;
	
	float rotateAmount;
	int lastRotated;
};
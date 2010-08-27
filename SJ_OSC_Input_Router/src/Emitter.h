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
#include <algorithm>
#include <string>

#define EMITTER_TIME 500


class Emitter : public ofRectangle
{
public:
	
	ofColor color;
	
	Emitter(){
		lastEmitted = ofGetElapsedTimeMillis();
		lastFoundString = 0;
		bSendSound = false;
		emitDelay= EMITTER_TIME;
		width = 234;
		height = 232;
		color.r = color.g = color.b = 50;
		bSetFont = false;
		bSent = false;
	};
	
	ofTrueTypeFont * font;
	bool bSetFont;
	
	void setFont( ofTrueTypeFont * _font){
		bSetFont = true;
		font = _font;
	}
	
	void setSendSound( bool _bSendSound = true ){
		bSendSound = _bSendSound;
	}
	
	bool sendSound(){
		return bSendSound;
	}
	
	void addMessageString( string msg ){
		messageStrings.push_back(msg);
	}
	
	void setName (string _name){
		name = _name;
		upperName = name;
		std::transform(upperName.begin(), upperName.end(),upperName.begin(), ::toupper);
	}
	
	string getName(){
		return name;
	}
	
	void setEmitDelay( int _emitDelay ){
		emitDelay = _emitDelay;
	}
	
	void update(){
		
	};
	
	void draw(){
		if (!bSent) ofSetColor(color.r, color.g, color.b);
		else ofSetColor(150,150,50);
		ofRect(x,y,width,height);
		ofSetColor(0xffffff);
		
		string drawString = upperName+"\n";
		string message = "";
		for (int i=0; i<messageStrings.size(); i++){
			message += messageStrings[i]+"\n";
		}
		
		font->drawString(drawString, x+10, y+20);
		ofSetColor(0xcccccc);
		ofPushMatrix();
		ofTranslate(x+10, y+40);
		ofScale(.75,.75);
		font->drawString(message, 0, 0);
		ofPopMatrix();
		
		bSent = false;
	};
	
	bool emit( int index = 0){
		if (ofGetElapsedTimeMillis() - lastEmitted > emitDelay){
			lastEmitted = ofGetElapsedTimeMillis();
			bSent = true;
			return true;
		}
		bSent = false;
		cout <<"too soon!"<<endl;
		return false;
	}
		
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
		
private:
	int lastEmitted;
	string name;
	string upperName;
	bool bSendSound;
	bool bSent;
	int emitDelay;
};
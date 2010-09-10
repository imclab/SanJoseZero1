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
#include "ofxOsc.h"
#include <algorithm>
#include <string>

#define EMITTER_TIME 500

class Message
{
public:
	
	string messageString;
	string data;
	int time;
	
	Message(){
		data = "";
	}
};

class Emitter : public ofRectangle
{
public:
	
	ofColor color;
	vector<Message> currentMessages;
	bool bEmitted;
	
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
		bEmitted = false;
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
	
	bool bHasWaitingMessages(){
		return (currentMessages.size() > 0);
	};
	
	Message nextMessage(){
		cout<<"get next?"<<endl;
		Message m;
		if (currentMessages.size() > 0){
			m = currentMessages[0];
			currentMessages.erase(currentMessages.begin());
		} 
		return m;
	};
	
	void update(){
		//cout<<currentMessages.size()<<endl;
		for (int i=currentMessages.size()-1; i>=0; i--){
			// greater than 3 queued, basically
			if (ofGetElapsedTimeMillis() - currentMessages[i].time > 1500){
				currentMessages.erase(currentMessages.begin() + i);
			};
		};
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
	
	bool emit(){
		if (ofGetElapsedTimeMillis() - lastEmitted > emitDelay){
			lastEmitted = ofGetElapsedTimeMillis();
			bSent = true;
			return true;
		}
		bSent = false;
		return false;
	}
	
	bool emit( ofxOscMessage m ){
		if (ofGetElapsedTimeMillis() - lastEmitted > emitDelay){
			lastEmitted = ofGetElapsedTimeMillis();
			if (currentMessages.size() <= 0){
				bSent = true;
				return true;
			}
		}
		Message newMessage;
		newMessage.messageString = m.getAddress();
		newMessage.time = ofGetElapsedTimeMillis();
		if (m.getNumArgs() > 0){
			newMessage.data = m.getArgAsString(0);
		} 
		currentMessages.push_back(newMessage);
		
		cout<<currentMessages.size()<<endl;
		
		bSent = false;
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
/*
 *  FoursquareSearcher.h
 *  SJTwitterCatcher
 *
 *  Created by brenfer on 9/3/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxHttpUtils.h"

class FoursquareSearcher
{
public:
	
	string name, messageString;	
	string searchUrl;
	string venueId;
	
/***********************************************************
	CONSTRUCTOR + SETUP
***********************************************************/
	
	FoursquareSearcher(){
		name = "";
		searchUrl ="";
		messageString = "/pluginplay/foursqure";
		searchTime = 2;
		venueId = "";
		setup();
	};
	
	FoursquareSearcher(string _name, string _messageString, string _url, ofxOscSender * _sender, int _searchTime ){
		name = _name;
		messageString = _messageString;
		searchUrl = _url;
		sender = _sender;
		searchTime = _searchTime;
		venueId = "";
		setup();
	};
	
	void setup(){
		bSearching = false;
		xmlLoaded =false;
		xmlResponse.addTag("results");
		
		replySearcher.start();
		ofAddListener(replySearcher.newResponseEvent,this,&FoursquareSearcher::replyResponse);
		
		bFirstQuery = true;
		bNewQuery = true;
		bOKtoSend = false;
		maxNumResults = curResult = 0;		
		lastID = "";
		
		lastSearchTime = ofGetElapsedTimeMillis();
	}
	
	void draw(int x, int y){
		string debugString = name;
		if (bSearching) debugString += ": searching ... \n";
		else debugString += ": waiting \n";
		debugString += ofToString(maxNumResults)+" checkins queued\n\n";
		debugString += "ready to send checkin "+ofToString(curResult)+"\n";
		debugString += "last id sent = "+lastID+"\n";
		debugString += "last id received = "+lastIDReceived+"\n";
		if (hashTags.size() > 0) debugString += "\nsearching tags:\n";
			
		for (int i=0; i<hashTags.size(); i++){
			debugString += hashTags[i]+"\n";
		}
		
		ofDrawBitmapString(debugString, x, y);
	};
	
	void attachSender( ofxOscSender * _sender){
		sender = _sender;
	}
	
	void setSearchTime( float _searchTime){
		searchTime = _searchTime;
	}
	
/***********************************************************
	 TAGS
***********************************************************/
	
	void clearTags(){
		hashTags.clear();
	}
	
	void addTag( string tag){
		hashTags.push_back(tag);
	}
	
	int getNumTags(){
		return hashTags.size();
	}
	
	bool bReadyToSearch(){
		return ofGetElapsedTimeMillis() - lastSearchTime >= searchTime;// && curResult == maxNumResults;
	}
	
/***********************************************************
	EXECUTE SEARCH
***********************************************************/
	
	void doSearch(){
		bSearching = true;
		ofxHttpForm form;
		form.action = searchUrl;  
		form.method = OFX_HTTP_POST;		
		form.clearFormFields();
		
		// Get @replies
		form.addFormField("vid",venueId);
		if (lastID != "") {
			ofLog(OF_LOG_VERBOSE, "adding reply id: "+lastID);
			form.addFormField("mc",lastID);
		} else {
			form.addFormField("mc","1");
		}
		ofLog(OF_LOG_VERBOSE,  "Searching venue "+venueId+" ....\n");
		replySearcher.addForm(form);
		
		//reset time
		
		lastSearchTime = ofGetElapsedTimeMillis();
	};
	
/***********************************************************
	 CALLBACK
***********************************************************/
	
	void replyResponse(ofxHttpResponse &response){
		bSearching = false;
		
		//to prevent our response getting overwritten if there's bad data
		ofxXmlSettings dummyRespose;
		dummyRespose.loadFromBuffer(response.responseBody);
		
		ofLog(OF_LOG_VERBOSE, "response:\n " + response.responseBody );
		
		bool bCorrectFormatting = dummyRespose.pushTag("results");
		if (!bCorrectFormatting){
			//cout<<"got blank response"<<endl;
			return;
		}else{ 
			//xmlResponse.loadFromBuffer(response.responseBody);			
			bool bcurResultExists = dummyRespose.pushTag("checkin",0);
			
			//are there tweets in this result?
			if (bcurResultExists){				
				//only get the last ID once! first tweet = most recent
				lastID = dummyRespose.getValue("timestamp","");
				dummyRespose.popTag();
				
				responses.push_back(response);
			}
			
		}
	};
	
/***********************************************************
	 SEND OSC
***********************************************************/
	
	void sendOSCSetup(){
		ofLog(OF_LOG_VERBOSE, "TRYING TO SEND "+ofToString(curResult)+ ":"+ofToString(maxNumResults));
		//if (!bOKtoSend) return;
		if (curResult == maxNumResults) {
			// SEE IF WE NEED TO GET ANOTHER RESPONSE SET FROM responses VECTOR
			if (getNextResponseSet()){
				sendOSC();
			} else {
				bNewQuery = true;
				bOKtoSend = false;
			}
		} else {
			sendOSC();
		}
		
	};
	
	void sendOSC(){				
		bool bHasResults = xmlResponse.pushTag("results");{
			if (!bHasResults) return;
					
			bool bcurResultExists = xmlResponse.pushTag("checkin",curResult);
			
				if (!bcurResultExists) return;
				
				ofxOscMessage m;
				m.setAddress(messageString);
				m.addStringArg(xmlResponse.getValue("timestamp","null") + "&*!" + xmlResponse.getValue("person_name","null") + "&*!" + xmlResponse.getValue("venue_name","null"));
				sender->sendMessage(m);	
				//cout<<"sending "<<name<<" osc "<<xmlResponse.getValue("timestamp","null")<<endl;
								
			xmlResponse.popTag();
		} xmlResponse.popTag();
		
		curResult++;
	};
	
	bool getNextResponseSet()
	{
		if (responses.size() > 0) {			
			bool bLoaded = xmlResponse.loadFromBuffer(responses[0].responseBody);
			if (!bLoaded){
				return false;
			}
			saveSettings();
			xmlResponse.pushTag("results");
			maxNumResults = xmlResponse.getNumTags("checkin");
			xmlResponse.popTag();
			
			curResult = 0;
			bOKtoSend = true;
			
			// Remove oldest entry
			responses.erase(responses.begin());
			
			return true;
		} else
			return false;
	};

/***********************************************************
	SAVE OUT DUMMY XML
***********************************************************/
	
	void saveSettings(){		
		//having issues saving when file is blank
		if (xmlResponse.tagExists("results")){		
			xmlResponse.saveFile(name+".xml");
		}
	}
	
	string lastID;
	bool bNewQuery;
		
private:
	
	bool xmlLoaded;
	
	ofxHttpUtils replySearcher;	
	ofxOscSender * sender;		
	ofxXmlSettings xmlResponse;
	
	vector<ofxHttpResponse> responses;
	
	int maxNumResults;	
	int curResult;
	float searchTime;
		
	// Stores the hash tags to search for
	vector<string> hashTags;
		
	time_t lastSearchTime;	
	
	bool bFirstQuery;
	bool bOKtoSend;	
	
	//some debug stuff
	string lastIDReceived;
	bool bSearching;
};
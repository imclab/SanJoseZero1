/*
 *  FlickrSearcher.h
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

class FlickrSearcher
{
public:
	
	string name, messageString;	
	string searchUrl;
	
/***********************************************************
	CONSTRUCTOR + SETUP
***********************************************************/
	
	FlickrSearcher(){
		name = "";
		searchUrl ="";
		messageString = "/pluginplay/twitter";
		searchTime = 2;
		setup();
	};
	
	FlickrSearcher(string _name, string _messageString, string _url, ofxOscSender * _sender, int _searchTime ){
		name = _name;
		messageString = _messageString;
		searchUrl = _url;
		sender = _sender;
		searchTime = _searchTime;
		setup();
	};
	
	void setup(){
		bSearching = false;
		xmlLoaded =false;
		xmlResponse.addTag("results");
		
		replySearcher.start();
		ofAddListener(replySearcher.newResponseEvent,this,&FlickrSearcher::replyResponse);
		
		bFirstQuery = true;
		bNewQuery = true;
		bOKtoSend = false;
		maxNumOfResults = curResult = 0;		
		lastID = "";
		
		lastSearchTime = ofGetElapsedTimeMillis();
	}
	
	void draw(int x, int y){
		string debugString = name;
		if (bSearching) debugString += ": searching... \n";
		else debugString += ": waiting \n";
		if (curResult < maxNumOfResults){
			debugString += ofToString(maxNumOfResults)+" photo queued\n\n";
			debugString += "ready to send photo "+ofToString(curResult)+"\n";
		} else {
			debugString += "sent all available data\n";
		}
		debugString += "last id sent = "+lastID+"\n";
		debugString += "last id received = "+lastIDReceived+"\n";
		if (tags.size() > 0) debugString += "\nsearching tags:\n";
			
		for (int i=0; i<tags.size(); i++){
			debugString += tags[i]+"\n";
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
		tags.clear();
	}
	
	void addTag( string tag){
		tags.push_back(tag);
	}
	
	int getNumTags(){
		return tags.size();
	}
	
	bool bReadyToSearch(){
		return ofGetElapsedTimeMillis() - lastSearchTime >= searchTime;// && curResult == maxNumOfResults;
	}
	
/***********************************************************
	GEO
***********************************************************/
	
	void setGeoParams( string _radius, string _lat, string _long ){
		radius = _radius;
		lat = _lat;
		longitude = _long;
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
		
		// Get tags
		if (tags.size() > 0){
			
			form.addFormField("gsearch","0");
			if (lastID != "") {
				ofLog(OF_LOG_VERBOSE, "adding time id: "+ lastID );		
				form.addFormField("ts",lastID);
			} else {
				form.addFormField("ts","1");
			}
			// ADD TAGS
			for (int i = 0; i < tags.size(); i++) {
				form.addFormField("tag[" + ofToString(i) + "]",tags[i]);
			}
			ofLog(OF_LOG_VERBOSE, "Getting tagged photos....\n");
			replySearcher.addForm(form);
			
		//get nearby photos
		} else {
						
			if (lastID != "") {
				ofLog(OF_LOG_VERBOSE, "adding time id: "+lastID);
				form.addFormField("ts",lastID);
			} else {
				form.addFormField("ts","1");
			}
			
			form.addFormField("gsearch","1");
			form.addFormField("radius",radius);
			form.addFormField("long",longitude);
			form.addFormField("lat",lat);
			ofLog(OF_LOG_VERBOSE,  "Getting nearby photos....\n");
			replySearcher.addForm(form);
		}
		
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
			cout<<"got blank response"<<endl;
			return;
		}else{ 
			//xmlResponse.loadFromBuffer(response.responseBody);			
			bool bcurResultExists = dummyRespose.pushTag("photo",0);
			
			//are there photos in this result?
			if (bcurResultExists){				
				//only get the last ID once! first photo = most recent
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
		ofLog(OF_LOG_VERBOSE, "TRYING TO SEND "+ofToString(curResult)+ ":"+ofToString(maxNumOfResults));
		//if (!bOKtoSend) return;
		if (curResult == maxNumOfResults) {
			// SEE IF WE NEED TO GET ANOTHER RESPONSE SET FROM responses VECTOR
			if (getNextResponseSet()) {
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
					
			bool bcurResultExists = xmlResponse.pushTag("photo",curResult);
			
				if (!bcurResultExists) return;
				
				ofxOscMessage m;
				m.setAddress(messageString);
				m.addStringArg(xmlResponse.getValue("timestamp","null") + "&*!" + xmlResponse.getValue("user_id","null") + "&*!" + xmlResponse.getValue("title","null")+ "&*!" + xmlResponse.getValue("link","null"));
				sender->sendMessage(m);	
				cout<<"sending "<<name<<" osc "<<xmlResponse.getValue("timestamp","null")<<endl;
								
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
			maxNumOfResults = xmlResponse.getNumTags("photo");
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
	string radius, lat, longitude;	
	
private:
	
	bool xmlLoaded;
	
	ofxHttpUtils replySearcher;	
	ofxOscSender * sender;		
	ofxXmlSettings xmlResponse;
	
	vector<ofxHttpResponse> responses;
	
	int maxNumOfResults;	
	int curResult;
	float searchTime;
		
	// Stores the hash tags to search for
	vector<string> tags;
		
	time_t lastSearchTime;	
	
	bool bFirstQuery;
	bool bOKtoSend;	
	
	//some debug stuff
	string lastIDReceived;
	bool bSearching;
};
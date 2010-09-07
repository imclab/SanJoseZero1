/*
 *  TwitterSearcher.h
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

class TwitterSearcher
{
public:
	
	string name, messageString;	
	string searchUrl, latestTweet;
	
/***********************************************************
	CONSTRUCTOR + SETUP
***********************************************************/
	
	TwitterSearcher(){
		name = "";
		searchUrl ="";
		messageString = "/pluginplay/twitter";
		searchTime = 2;
		setup();
	};
	
	TwitterSearcher(string _name, string _messageString, string _url, ofxOscSender * _sender, int _searchTime ){
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
		ofAddListener(replySearcher.newResponseEvent,this,&TwitterSearcher::replyResponse);
		
		bFirstQuery = true;
		bNewQuery = true;
		bOKtoSend = false;
		maxNumOfTweets = curTweet = 0;		
		lastID = "";
		
		lastSearchTime = ofGetElapsedTimeMillis();
	}
	
	void draw(int x, int y){
		string debugString = name;
		if (bSearching) debugString += ": searching... \n";
		else debugString += ": waiting \n";
		
		if (curTweet < maxNumOfTweets){
			debugString += ofToString(maxNumOfTweets)+" tweets queued\n\n";
			debugString += "ready to send tweet "+ofToString(curTweet)+"\n";
		} else {
			debugString += "sent all available data\n";
		}
		
		debugString += "last id sent = "+lastID+"\n";
		debugString += "last id received = "+lastIDReceived+"\n";
		debugString += latestTweet+"\n";
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
		return ofGetElapsedTimeMillis() - lastSearchTime >= searchTime;// && curTweet == maxNumOfTweets;
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
		
		// Get hashtags
		if (hashTags.size() > 0){
			
			form.addFormField("hash","1");
			if (lastID != "") {
				ofLog(OF_LOG_VERBOSE, "adding hash id: "+ lastID );		
				form.addFormField("ms",lastID);
			} else {
				form.addFormField("ms","1");
			}
			// ADD TAGS
			for (int i = 0; i < hashTags.size(); i++) {
				form.addFormField("tag[" + ofToString(i) + "]",hashTags[i]);
			}
			//	form.addFormField("ms","22734030265");
			ofLog(OF_LOG_VERBOSE, "Getting hashtags....\n");
			replySearcher.addForm(form);
			
		//get at replies
		} else {
			
			ofxHttpForm form;
			form.action = searchUrl;  //"http:www.plug-in-play.com/testing5.php";
			form.method = OFX_HTTP_POST;
			
			// Get @replies
			form.addFormField("hash","0");
			if (lastID != "") {
				ofLog(OF_LOG_VERBOSE, "adding reply id: "+lastID);
				form.addFormField("ms",lastID);
			} else {
				form.addFormField("ms","1");
			}
			ofLog(OF_LOG_VERBOSE,  "Getting @replies....\n");
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
			
			bool bCurTweetExists = dummyRespose.pushTag("tweet",0);
			
			//are there tweets in this result?
			if (bCurTweetExists){				
				//only get the last ID once! first tweet = most recent
				lastID = dummyRespose.getValue("tweet_id","");
				dummyRespose.popTag();
				
				responses.push_back(response);
				
				//get next set of responses! (mostly for the first time this loads up)
				if (curTweet == maxNumOfTweets)
					getNextResponseSet();
			} else {
				cout<<"no tweets!"<<endl;
			}
			
			dummyRespose.popTag();
			
		}
	};
	
/***********************************************************
	 SEND OSC
***********************************************************/
	
	bool hasWaitingResults(){
		return curTweet < maxNumOfTweets;
	};
	
	void sendOSCSetup(){
		ofLog(OF_LOG_VERBOSE, "TRYING TO SEND "+ofToString(curTweet)+ ":"+ofToString(maxNumOfTweets));
		//if (!bOKtoSend) return;
		if (curTweet == maxNumOfTweets) {
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
		ofLog(OF_LOG_VERBOSE, "sending "+name+" osc "+lastID);
		
		bool bHasResults = xmlResponse.pushTag("results");{
			if (!bHasResults) return;
					
			string type_of_result = xmlResponse.getAttribute("tweet","type","",curTweet);
			bool bCurTweetExists = xmlResponse.pushTag("tweet",curTweet);
			
				if (!bCurTweetExists) return;
				
				ofxOscMessage m;
				m.setAddress(messageString);
				m.addStringArg(xmlResponse.getValue("timestamp","null") + "&*!" + xmlResponse.getValue("user_name","null") + "&*!" + xmlResponse.getValue("content","null"));
				sender->sendMessage(m);	
			
			latestTweet = xmlResponse.getValue("user_name","null") +" : "+xmlResponse.getValue("content","null");
								
			xmlResponse.popTag();
		} xmlResponse.popTag();
		
		curTweet++;
	};
	
	bool getNextResponseSet()
	{
		if (responses.size() > 0) {			
			bool bLoaded = xmlResponse.loadFromBuffer(responses[0].responseBody);
			if (!bLoaded){
				return false;
			}
			//saveSettings();
			xmlResponse.pushTag("results");
			maxNumOfTweets = xmlResponse.getNumTags("tweet");
			xmlResponse.popTag();
			
			curTweet = 0;
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
	
	int maxNumOfTweets;	
	int curTweet;
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
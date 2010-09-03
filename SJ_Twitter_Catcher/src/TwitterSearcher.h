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
	string searchUrl;
	
	TwitterSearcher(){
		name = "";
		searchUrl ="";
		messageString = "/pluginplay/twitter";
		setup();
	};
	
	TwitterSearcher(string _name, string _messageString, string _url, ofxOscSender * _sender ){
		name = _name;
		messageString = _messageString;
		searchUrl = _url;
		sender = _sender;
		setup();
	};
	
	void setup(){
		xmlLoaded =false;
		xmlResponse.addTag("results");
		
		replySearcher.start();
		ofAddListener(replySearcher.newResponseEvent,this,&TwitterSearcher::replyResponse);
		
		bFirstQuery = true;
		bNewQuery = true;
		bOKtoSend = false;
		maxNumOfTweets = 0;
		curTweet = 0;
		
		lastID = "";
		
		time_t curTime;
		time(&curTime);
		lastSearchTime = curTime;
	}
	
	void attachSender( ofxOscSender * _sender){
		sender = _sender;
	}
	
	void clearTags(){
		hashTags.clear();
	}
	
	void addTag( string tag){
		hashTags.push_back(tag);
	}
	
	int getNumTags(){
		return hashTags.size();
	}
	
	void doSearch(){
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
			cout << "Getting @replies....\n";
			replySearcher.addForm(form);
		}
	};
	
	void replyResponse(ofxHttpResponse &response){
		//to prevent our response getting overwritten if there's bad data

		ofxXmlSettings dummyRespose;
		dummyRespose.loadFromBuffer(response.responseBody);
		
		cout << "response:\n " << response.responseBody << endl;
		
		bool bCorrectFormatting = dummyRespose.pushTag("results");
		if (!bCorrectFormatting){
			return;
		}else{ 
			xmlResponse.loadFromBuffer(response.responseBody);
		}
		
		cout << "attribute: " << dummyRespose.getAttribute("tweet","type","void");
		dummyRespose.popTag();
		cout << endl << endl << endl;
		
		if (!responses.empty()) {
			responses.push_back(response);
		} else {			
			bool bHasResults = xmlResponse.pushTag("results");
			if (bHasResults){
				xmlLoaded = true;
				maxNumOfTweets = xmlResponse.getNumTags("tweet");
				xmlResponse.popTag();
				
				curTweet = 0;
				if (curTweet < maxNumOfTweets) {
					bOKtoSend = true;
				} else {
					bOKtoSend = false;
					bNewQuery = true;
				}
			} else { 
				cout<<"returned blank result"<<endl;
			}
		}
	};
	
	void sendOSCSetup(){
		cout<<"TRYING TO SEND "<<curTweet<<":"<<maxNumOfTweets<<endl;
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
		cout<<"sending "<<name<<" osc "<<lastID<<endl;
		bool bHasResults = xmlResponse.pushTag("results");
		if (!bHasResults) return;
		
		cout<<"sending "<<curTweet<<endl;
		
		string type_of_result = xmlResponse.getAttribute("tweet","type","",curTweet);
		bool bCurTweetExists = xmlResponse.pushTag("tweet",curTweet);
		
		if (!bCurTweetExists) return;
		
		ofxOscMessage m;
		m.setAddress(messageString);
		m.addStringArg(xmlResponse.getValue("timestamp","null") + "&*!" + xmlResponse.getValue("user_name","null") + "&*!" + xmlResponse.getValue("content","null"));
		sender->sendMessage(m);	
		
		lastID = xmlResponse.getValue("tweet_id","");
		
		xmlResponse.popTag();
		xmlResponse.popTag();
		
		curTweet++;
	};
	
	bool getNextResponseSet()
	{
		if (responses.size() > 1) {
			// Remove oldest entry
			responses.erase(responses.begin());
			
			xmlResponse.loadFromBuffer((responses[0]).responseBody);
			xmlResponse.pushTag("results");
			maxNumOfTweets = xmlResponse.getNumTags("tweet");
			xmlResponse.popTag();
			
			curTweet = 0;
			bOKtoSend = true;
			return true;
		} else
			return false;
	};

	void saveSettings(){		
		//having issues saving when file is blank
		if (xmlLoaded && xmlResponse.tagExists("results")){			
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
		
	// Stores the hash tags to search for
	vector<string> hashTags;
		
	time_t lastSearchTime;	
	
	bool bFirstQuery;
	bool bOKtoSend;	
	
};
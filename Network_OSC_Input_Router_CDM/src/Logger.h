#ifndef _LOGGER_H
#define _LOGGER_H



/*
 *  Logger.h
 *  openTSPS
 *
 *  Created by Zack Boka on 9/6/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxHttpUtils.h"
#include "ofxXmlSettings.h"
#include <time.h>

class Logger {
public:

/********************************************
	CONSTRUCTOR / SETUP
*********************************************/
	Logger(int _threshold, string _url) {
		logNumParticles = 0;
		
		postURL = _url;
		
		xmlGetter.start();
		ofAddListener(xmlGetter.newResponseEvent,this,&Logger::response);
		
		logDelayThreshold = _threshold;
		lastEmitTime = ofGetElapsedTimeMillis();

		// Set the sDate variable
		int month = ofGetMonth();
		int day = ofGetDay();
		if (month < 10) {
			sDate = "0";
		}
		sDate += ofToString(month);
		if (day < 10) {
			sDate += "0";
		}
		sDate += ofToString(day);
									

		
		// Lastly, set the curEmitID variable
		getCurEmitID();
	};
	
	

/********************************************
	DATA SENDING / LOGGING FUNCTION
*********************************************/
	void logData() {
		cout << "logging\n";
		
		int curElapsedTime = ofGetElapsedTimeMillis() - lastEmitTime;
		logNumParticles++;
		
		if ( curElapsedTime <= logDelayThreshold) {
			// Too soon to log!
			return;
		} else {
			lastEmitTime = ofGetElapsedTimeMillis();
		}
		
		ofxHttpForm form;
		form.action = postURL;
		form.method = OFX_HTTP_POST;
		
		form.addFormField("logData","1");
		form.addFormField("curEmitID",ofToString(++curEmitID));
		form.addFormField("date",sDate);
		form.addFormField("milliseconds",ofToString(curElapsedTime));
		form.addFormField("activeParticles",ofToString(logNumParticles));
		
		xmlGetter.addForm(form);
		
		// Reset the number of active particles to log
		logNumParticles = 0;
		
	};
	

/********************************************
	DATA RECEIVING FUNCTION
*********************************************/
	void response(ofxHttpResponse &response) {
		cout << "receiving\n";
		string sResults;
		
		ofxXmlSettings xmlResponse;
		xmlResponse.loadFromBuffer(response.responseBody);
		xmlResponse.copyXmlToString(sResults);
		cout << sResults << endl << endl;
		
		
		// Did we get a blank response?
		bool bCorrectFormatting = xmlResponse.pushTag("results");
		if (!bCorrectFormatting) {
			cout << "got blank response\n";
			return;
		}

		
		// Did we get back the current emit ID?
		if (xmlResponse.tagExists("curEmitID")) {
			curEmitID = xmlResponse.getValue("curEmitID",-1);
			cout << "curEmitID found: " << curEmitID << endl;
			

		// Did we get back errors?
		} else if (xmlResponse.tagExists("error")) {
			cout << "ERROR: " << xmlResponse.getValue("error","");
		}
	}

	
	
private:
	string postURL;
	ofxHttpUtils xmlGetter;
	int curEmitID;
	string sDate;
	int lastEmitTime;
	int logDelayThreshold;
	int logNumParticles;

	

	// GET THE CURRENT EMIT ID
	void getCurEmitID() {
		ofxHttpForm form;
		form.action = postURL;
		form.method = OFX_HTTP_POST;
		
		form.addFormField("getCurEmitID","1");
		
		xmlGetter.addForm(form);
	};
	
	
	
	
	
};

#endif
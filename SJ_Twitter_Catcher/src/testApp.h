#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxHttpUtils.h"

#include <time.h>
#include <stdio.h>

#define SEARCH_TIME_SECONDS 5
#define FORWARD_TIME_SECONDS 1
#define UPDATE_HASHTAGS_SECONDS 5

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void doSearch();
		void infoResponse(ofxHttpResponse &response);
		void sendOSCSetup();
		void sendOSC(); // DO NOT CALL THIS METHOD DIRECTLY!  JUST CALL sendOSCSetup()
		bool getNextResponseSet();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		
	
	ofxHttpUtils infoGetter;
	
	ofxOscSender sender;
	
	ofxXmlSettings xmlResponse;
	vector<ofxHttpResponse> responses;
	int maxNumOfTweets;
	string lastHashID;
	string lastReplyID;
	int curTweet;
	
	// Stores the hash tags to search for
	vector<string> hashTags;
//	int curNumOfTags;
	time_t lastHashtagsUpdateTime;
	
	time_t lastSearchTime;
	time_t lastForwardTime;
	
	
	bool bFirstQuery;
	bool bNewQuery;
	bool bOKtoSend;
	
	
};

#endif

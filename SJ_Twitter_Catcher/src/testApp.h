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

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void doSearch();
		void infoResponse(ofxHttpResponse &response);
		void sendOSC();

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
	int maxNumOfTweets;
	int curTweet;
	string curTimestamp;
	
	
	time_t lastSearchTime;
	time_t lastForwardTime;
	
	
	bool bFirstQuery;
	bool bNewQuery;
	bool bOKtoSend;
};

#endif

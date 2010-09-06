#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxHttpUtils.h"

#include "FlickrSearcher.h"

#include <time.h>
#include <stdio.h>

#define SEARCH_TIME_SECONDS 4
#define FORWARD_TIME_SECONDS 1
#define UPDATE_HASHTAGS_SECONDS 4

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		
		FlickrSearcher * tagSearcher;
		FlickrSearcher * nearbySearcher;
	
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
		void saveSettings();
				
		string searchUrl;
		int logLevel;
		float searchTime;
		float sendTime;
	
		ofxOscSender sender;

		//message strings to send
		vector <string> messageStrings;
	
		int lastHashtagsUpdateTime;	
		int lastSearchTime;
		int lastForwardTime;
		
		ofTrueTypeFont font;
};

#endif

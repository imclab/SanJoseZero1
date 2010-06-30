#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxFourSquare.h"

//City hall
//http://foursquare.com/venue/61626

//RG
//http://foursquare.com/venue/101107

//#define CHECK_TIME 18
#define CHECK_TIME 18
#define VENUE_ID "101107"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		
		int lastTimeChecked;
	
		ofxFourSquare venueDetails;
		ofxFourSquare radiusSearch;
		ofxXmlSettings currentXML;
		bool bChecked;
	
		string queryLat, queryLong;
		ofxOscSender sender;
};

#endif

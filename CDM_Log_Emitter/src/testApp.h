#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxHttpUtils.h"
#include "ofxOsc.h"
#include "ofxThread.h"
#include "ofxXmlSettings.h"


class testApp : public ofBaseApp {

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
	
/*******************************************
	FUNCTIONS
*******************************************/

	// Getting chunks of XML (list of particles to emit)
	void getNextXMLchunk();
	void receivedNextXMLchunk(ofxHttpResponse &response);
	
	// Handle sending OSC messages
	void sendOSCMessage(string data);
	
	
/*******************************************
	VARIABLES
*******************************************/
	
	// Manage OSC Messages
	ofxOscSender OSCsender;
	string messageAddress;
	
	
	// Manage Http requests	
	ofxHttpUtils xmlGetter;
	string postURL;
	string sDate;
	bool bFirstTimeThrough;
	bool bNoMoreXML;
	bool bRequestingXML;
	bool bOKtoGetNextChunk;
	int highestEmitID;  // Keeps track of the highest emit ID we currently have in the resposneQueue
						//   since we must supply it to the php script to get a chunk of response XML
						//   containing successive <emission> tags.
	
	
	// Manage XML
//	list<ofxXmlSettings> resultsQueue;
	list<string> sResultsQueue;

	
	// Keep track of particles being emitted from current chunk of XML
	int curTotalEmissions;
	int curEmission;
	int curEmitID;
	
	
	// Keep track of time intervals
	int nextEmitInterval;
	int lastEmitTimeMillis;

	
	// LOGGING
	string LOG;
};

#endif

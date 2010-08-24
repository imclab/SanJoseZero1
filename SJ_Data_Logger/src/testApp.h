#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxHttpUtils.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
	
		void windowResized(int w, int h);
		
		void newResponse(ofxHttpResponse & response);
		string responseString;
		
		ofxHttpUtils httpUtils;
		string postUrl;
		ofxOscReceiver receiver;
};

#endif

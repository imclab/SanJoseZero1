#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "Emitter.h"
#include "Logger.h"
#define NUM_STRINGS 10

//--------------------------------------------------------
class testApp : public ofBaseApp{
	
public:
	
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
		
	vector<Emitter *>		emitters;
	
private:
	bool			bDrawDebug;
	ofxOscReceiver	receiver;
	ofxOscSender	sender;
	ofxOscSender	soundSender;
	
	string debugString [10];
	string errorString [10];
	
	ofTrueTypeFont font;
	
	int boxX, boxY;


	// ZACK BOKA
	// Logging particle emissions for later playback at the SJ City Hall location
	Logger* dataLogger;
	int curEmitID;

};

#endif

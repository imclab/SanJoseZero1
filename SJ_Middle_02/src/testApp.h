#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "Emitter.h"
#include "ofx3DModelLoader.h"

// listen on port 12345
//#define HOST "localhost"
//#define PORT 12345

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
	
	void elementLeftScreen( ParticleEventArgs & args );
	
	ofTrueTypeFont			font;	
	vector<Emitter* >		emitters;
	
	// TESTING 3D STUFF
//	ofx3DModelLoader testHop;
	
private:
	bool			bDrawDebug;
	ofxOscReceiver	receiver;
	ofxOscSender	sender;
	
	int				mouseX, mouseY;
	string			mouseButtonState;
};

#endif

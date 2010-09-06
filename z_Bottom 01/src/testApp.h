#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "Emitter.h"

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
	vector<Emitter *>		emitters;	
	
private:
	bool			bDrawDebug;
	ofxOscReceiver	receiver;
	ofxOscSender	sender;
	ofxOscSender	soundSender;
	int				mouseX, mouseY;
	string			mouseButtonState;
	int				numReceivers;
};

#endif

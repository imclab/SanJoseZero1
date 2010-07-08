#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofx3DUtils.h"
#include "ofxOsc.h"
#include "Emitter.h"

#define PORT 12000

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
		
		Emitter * particleManager;
		
		ofxLight light1; //this will be a directional light
		ofxLight light2; //this one a spot light
		ofxLight light3; //and this one a point light
		
	
	private:
		ofxOscReceiver	receiver;
		ofxOscSender	sender;
	
};

#endif

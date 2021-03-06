#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxTSPS.h"
#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file
//#define USE_CUSTOM_GUI		// uncomment to add a "custom" panel to the gui
								// for adding your own parameters

#include "Hopscotch.h"
#include "Quad.h"

#include <time.h>


// DRAW_MODE_CYA -- Shouldn't be here?
enum {
	DRAW_MODE_CYA, DRAW_MODE_HOPSCOTCH
};


class testApp : public ofBaseApp, public ofxPersonListener {

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
	
		void personEntered( ofxTSPSPerson* newPerson, ofxTSPSScene* scene );
		void personMoved( ofxTSPSPerson* activePerson, ofxTSPSScene* scene );
		void personWillLeave( ofxTSPSPerson* leavingPerson, ofxTSPSScene* scene );
		void personUpdated( ofxTSPSPerson* updatedPerson, ofxTSPSScene* scene );
	
		void sendOscMessage(int quadIndex);
	
        #ifdef _USE_LIVE_VIDEO
		  ofVideoGrabber 		vidGrabber;
		#else
		  ofVideoPlayer 		vidPlayer;
		#endif
		int camWidth, camHeight;

		ofxCvColorImage		colorImg;	

		float* opticalFlowDetectionThreshold;
	
	
	//hopscotch
		int drawMode;
		Hopscotch hopscotch;
	
	//status bar stuff
		ofImage statusBar;
		int		drawStatus[3];
		ofImage personEnteredImage;
		ofImage personUpdatedImage;
		ofImage personLeftImage;
		ofTrueTypeFont timesBoldItalic;
	
	//other gui images
		ofImage background;


	ofxTSPSPeopleTracker peopleTracker;
	
	//example test values
	int itestValue;
	float ftestValue;
	bool btestValue;
	
};

#endif

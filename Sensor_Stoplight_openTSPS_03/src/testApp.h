#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxTSPS.h"
#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
								// otherwise, we'll use a movie file
//#define USE_CUSTOM_GUI		// uncomment to add a "custom" panel to the gui
								// for adding your own parameters

#include "Stoplight.h"
#include "Quad.h"

#include <time.h>


enum {
	DRAW_MODE_CYA, DRAW_MODE_STOPLIGHT
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

	
	//stoplight
		int drawMode;
		int lastSent;
		Stoplight stoplight;
	
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
	
	
	
	//ZACK BOKA: Stoplight-specific
	ofxCvColorImage warpedImage;						// get the image in color
	int theX,theY;										// the x- and y-coordinates for the 'color square'
	int r,g,b;											// the RGB values for the 'color sqaure'
	ofxColor redLight,yellowLight,greenLight;			// the pixel color representing each light
	int lightIndex;										// the light we are currently examining to trigger an OSC send signal event
	int colorSet;										// indicates the last color that was set by clicking inside its quad
	int colorDisplayCtr;								// Counter for how long to display the color that was just set
	float* colorSensingVarianceThreshold;				// Threshold for detecting color variance from the prespecified quad color (this variable set in GUI)
	
	
	
};

#endif

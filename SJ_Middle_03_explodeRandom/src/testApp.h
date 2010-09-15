#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxLabUtils.h"

#include "ofx3DUtils.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "Emitter.h"
#include "BuildingType.h"
#include "ofx3DModelLoader.h"
#include "ofxLabProjectionTools.h"
#include "ofxVectorMath.h"

//particle effects
#include "ParticleTrails.h"
#include "phyParticleSystem.h"

#include "Columns.h"

enum {
	LAB_MODE_RENDER,
	LAB_MODE_CALIBRATE,
	LAB_MODE_P_CALIBRATE
};

#define NUM_DRAW_MODES 8

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
	
	void setupGui();
	void saveSettings();
	
	//event to forward on particle to top app
	void elementLeftScreen( ParticleEventArgs & args );
	
	Emitter					particleManager;
	
	//TRAILS
	ParticleTrails trails;
	
// calibration
	bool bUseProjectionTools;
	ofTrueTypeFont			font;	
	int drawMode;
	ofColor					bgColor;
	ofxLabProjectionTools	projection;
	
	// PARTICLE EFFECTS
	phyParticleSystem *		system;
	ofxFBOTexture			explosionFBO;
	
	//lighting
	ofxLight light1; //this will be a directional light
	ofxLight light2; //this one a spot light
	ofxLight light3; //and this one a point light
	ofxVec3f lightPos;
	
private:
	bool			bDrawDebug;
	ofxOscReceiver	receiver;
	ofxOscReceiver	calibrationReceiver;
	ofxOscSender	sender;
	
	Columns			columns;
	int				mouseX, mouseY;
	string			mouseButtonState;
};

#endif

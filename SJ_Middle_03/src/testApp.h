#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
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

#define FLUID_EFFECT_SYSTEM
#ifdef FLUID_EFFECT_SYSTEM
#include "EffectsSystem.h"
#endif

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
	
	ofTrueTypeFont			font;	
	Emitter					particleManager;
		
	int drawMode;
	
	ofxLabProjectionTools	projection;
		
#ifdef FLUID_EFFECT_SYSTEM
	EffectsSystem			effectsSystem;
#endif
	
	//lighting
	ofxLight light1, light2;
	ofxVec3f lightPos;
	
	//particle effects
	ParticleTrails trails;
	
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

#pragma once

#include "MSAFluid.h"
#include "MSATimer.h"
#include "FluidParticleSystem.h"

#include "ofMain.h"

#include "Emitter.h"

// comment this line out if you don't wanna use TUIO
// you will need ofxTUIO & ofxOsc
//#define USE_TUIO		

// comment this line out if you don't wanna use the GUI
// you will need ofxSimpleGuiToo, ofxMSAInteractiveObject & ofxXmlSettings
// if you don't use the GUI, you won't be able to see the fluid parameters
//#define USE_GUI		

#ifdef USE_GUI 
#include "ofxSimpleGuiToo.h"
#endif

using namespace MSA;

class EffectsSystem{
public:
	void setup(Emitter * _e);
	void update();
	void draw();
	
	void keyPressed  (int key);
	
	bool windowResized(int w, int h);
	
	void fadeToColor(float r, float g, float b, float speed);
	void addToFluid(Vec2f pos, Vec2f vel, ofColor color, bool addColor, bool addForce);
	
	int					fluidCellsX;
	bool				resizeFluid;
	bool				drawFluid;
	bool				drawParticles;
	
	float getWindowAspectRatio(){	
		return window.x * 1.0f / window.y; 
	};
	
	ofPoint loc, window;
	
	bool showSettings;
	
	MSA::FluidSolver	fluidSolver;
	MSA::FluidDrawerGl	fluidDrawer;	
	
	FluidParticleSystem		particleSystem;
	
	Emitter * emitter;
	
#ifdef USE_GUI 
	ofxSimpleGuiToo	gui;
#endif
	
	
};




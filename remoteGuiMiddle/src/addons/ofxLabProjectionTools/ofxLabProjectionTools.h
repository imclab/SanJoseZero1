/*
 *  ofxLabProjectionTools.h
 *  kaleidoscope
 *
 *  Created by Brett Renfer on 3/16/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#define USE_LAB_GUI //comment this out to not use the gui
//#define USE_SHADERS //comment this out to not use shaders
#define USE_ADV_MODE //uncomment this to use more powerful gui (but with less clear verbage)

#include "ofxLabProjectedView.h"
#include "ofxXmlSettings.h"

#ifdef USE_LAB_GUI
#include "ofxLabGui.h"
#endif

#ifdef USE_SHADERS
#include "ofxShader.h"
#endif

enum {
	LAB_DRAW_MODE_RENDER, 
	LAB_DRAW_MODE_MOVE,
	LAB_DRAW_MODE_CROP,
	LAB_DRAW_MODE_SKEW,
	LAB_DRAW_MODE_MASK,
	LAB_DRAW_MODE_GUI
};
#define NUM_MODES 6

/**************************************************************************

 this is a set of tools for creating masked projections
 right now, it works like this:
 - add views in setup()
 - in draw(), push and pop views to draw to them
 - when you're done drawing to each view, call draw() on 
	your ofxLabProectionTools object
 
 other utils:
 - gui
	- an ofxLabGui instance is rolled in
	- to add to it, call getGui() in your testApp, which returns a pointer
		to the gui
	- you can add default groups + panels as well to match look + feel
 
**************************************************************************/

class ofxLabProjectionTools
{
public:
	
	ofxLabProjectionTools();
	
/**************************************************************************
	 MAIN DRAWING FUNCTIONS
**************************************************************************/
	
#ifdef USE_LAB_GUI
	void setupGui();
#endif
	
	ofxLabProjectedView * addView( int x, int y, int width, int height );
	void pushView();
	void pushView( int which );
	void popView();
	void update();
	void draw();
	void draw( int which );

/**************************************************************************
	 IMAGE ADJUSTMENT SHADERS
**************************************************************************/
	
#ifdef USE_SHADERS
	ofxShader amplify;
	float amplifyAmount;
#endif
	
/**************************************************************************
	 VIEW INFORMATION
**************************************************************************/
	
	ofPoint getViewResolution( int which );	
	ofPoint getRenderPosition( int which );
	
/**************************************************************************
	DRAW SKEWABLE CORNERS
**************************************************************************/
	
	void drawDebug( bool debug=true); // deprecated
	void toggleDrawDebug();	// deprecated
	void drawSkew( bool skew=true);
	void toggleDrawSkew();
	
/**************************************************************************
	DRAW CROP CORNERS
**************************************************************************/
	
	void drawCrop( bool crop=true);
	void toggleDrawCrop();
	
/**************************************************************************
	DRAW OVERLAYS
**************************************************************************/
	
	void drawOverlays( bool overlays=true);
	
/**************************************************************************
	GUI CONTROLS
**************************************************************************/
	
#ifdef USE_LAB_GUI
	void drawGui( bool gui=true);
	void toggleDrawGui();
	ofxLabGui * getGui();
	guiTypePanel * addDefaultPanel( string name );
	guiTypeGroup * addDefaultGroup ( string name, bool showText=false);	
	void loadGuiSettings();
#endif
	
/**************************************************************************
	UTILS
**************************************************************************/
	
	// VIEWS ----------------------------------------------------------------------------
	ofxFBOTexture screen;
	ofxLabProjectedView * getView( int which );
	int getNumViews();
	
	// MOUSE ----------------------------------------------------------------------------
	void mousePressed( ofMouseEventArgs & e );
	void mouseReleased( ofMouseEventArgs & e );
	void mouseDragged( ofMouseEventArgs & e );
	
	void windowResized( ofResizeEventArgs & e );
		
	// KEY ----------------------------------------------------------------------------
	void enableModes();
	void disableModes();
	void toggleEnableModes();
	void keyPressed( ofKeyEventArgs & e );
	
	// SETTINGS ----------------------------------------------------------------------------
	void loadSettings();
	void saveSettings();
		
private:	
	
#ifdef USE_LAB_GUI
	ofxLabGui gui;
#endif
	bool bDrawGui;
	bool bDrawSkew;
	bool bDrawCropping;
	bool bDrawOverlays;
	
	bool bEnableDrawmodes;
	int drawMode;
	ofTrueTypeFont modeFont;
	
	vector<ofxLabProjectedView *> views;
	int currentView;
	ofxXmlSettings settings;
	
};


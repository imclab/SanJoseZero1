/*
 *  ofxLabProjectionTools.cpp
 *  NeverendingForest
 *
 *  Created by brenfer on 7/14/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#include "ofxLabProjectionTools.h"

ofxLabProjectionTools::ofxLabProjectionTools(){
	currentView = -1;
	bDrawGui = false;
	bDrawSkew = false;
	bDrawOverlays = false;
	bDrawCropping = false;
	
#ifdef USE_LAB_GUI 
	setupGui();
#endif
	
	ofAddListener(ofEvents.mousePressed, this, &ofxLabProjectionTools::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &ofxLabProjectionTools::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &ofxLabProjectionTools::mouseDragged);
	ofAddListener(ofEvents.keyPressed, this, &ofxLabProjectionTools::keyPressed);
	ofAddListener(ofEvents.windowResized, this, &ofxLabProjectionTools::windowResized);
	
#ifdef USE_SHADERS
	amplify.setup("shaders/amplify");
#endif
	
	bEnableDrawmodes = false;
	drawMode = 0;
	
	//load font for modes
	modeFont.loadFont("fonts/futura_bold.ttf", 20);
	
	loadSettings();
	
	//allocate screen FBO (for shaders)
	screen.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
};


//----------------------------------------------------------------------------

#ifdef USE_LAB_GUI
void ofxLabProjectionTools::setupGui(){
#ifndef USE_ADV_MODE
	gui.setup("settings", 0, 0, 500, 500);
#else
	gui.setup("settings", 0, 0, 700, 700);
#endif
	gui.setShowText(false);
	gui.setCollapsible(false);
	gui.setDraggable(true);
	gui.setBackgroundColor(255,255,255);
	gui.setOutlineColor(255,255,255,0);
	gui.setTextColor(148,129,85);
	gui.setSaveColor(58, 187, 147);
	gui.setLoadColor(180, 87, 128);
	gui.setSaveAsColor(238, 53, 35);
	gui.setSaveSelectedColor(80, 253, 190);
	gui.setRestoreColor(34, 151, 210);
	gui.setRestoreSelectedColor(116, 191, 228);
	gui.setDrawBarOnTop(false);		
	
	addDefaultPanel("settings");
	addDefaultGroup("image settings", true);
	gui.addSlider("amplify", "GUI_AMP", 1.0, .1, 2.0, false);
	gui.update();
}

void ofxLabProjectionTools::loadGuiSettings(){
	gui.loadSettings("settings/guisettings.xml");
};

#endif

//----------------------------------------------------------------------------
ofxLabProjectedView * ofxLabProjectionTools::addView( int x, int y, int width, int height ){
	ofxLabProjectedView * view = new ofxLabProjectedView(views.size(), x,y,width,height);
	views.push_back(view);
	
	stringstream name;
	name<<"edges "<<views.size();
	
	stringstream index;
	index<<views.size()-1;
	
#ifdef USE_LAB_GUI
	
	
#ifndef USE_ADV_MODE
	guiTypePanel * panel = addDefaultPanel("refine edges");
	
	// video settings
	gui.setWhichPanel(name.str());
	addDefaultGroup("soften edges");	
	gui.addSlider("top", "TOP_MASK_HEIGHT_"+index.str(), view->edges[0].height,0,view->height,false);
	gui.addSlider("bottom", "BOTTOM_MASK_HEIGHT_"+index.str(), view->edges[2].height,0,view->height,false);
	gui.addSlider("left", "LEFT_MASK_HEIGHT_"+index.str(), view->edges[3].height,0,view->width,false);
	gui.addSlider("right", "RIGHT_MASK_HEIGHT_"+index.str(), view->edges[1].height,0,view->width,false);	
#else
	
	guiTypePanel * panel = addDefaultPanel(name.str()+" TB");
	
	// video settings
	gui.setWhichPanel(name.str());
	addDefaultGroup("top mask");
	gui.addSlider("top mask: x", "TOP_MASK_X_"+index.str(), view->edges[0].x,-view->width,view->width,false);
	gui.addSlider("top mask: y", "TOP_MASK_Y_"+index.str(), view->edges[0].y,-view->height,view->height,false);
	gui.addSlider("top mask: width", "TOP_MASK_WIDTH_"+index.str(), view->edges[0].width,0,view->width,false);
	gui.addSlider("top mask: height", "TOP_MASK_HEIGHT_"+index.str(), view->edges[0].height,0,view->height,false);
	gui.addSlider("top mask: rotation", "TOP_MASK_ROTATE_"+index.str(), view->edges[0].rotation,-180,180,false);
	addDefaultGroup("bottom mask");
	gui.addSlider("bottom mask: x", "BOTTOM_MASK_X_"+index.str(), view->edges[2].x,-view->width,view->width,false);
	gui.addSlider("bottom mask: y", "BOTTOM_MASK_Y_"+index.str(), view->edges[2].y,-view->height,view->height,false);
	gui.addSlider("bottom mask: width", "BOTTOM_MASK_WIDTH_"+index.str(), view->edges[2].width,0,view->width,false);
	gui.addSlider("bottom mask: height", "BOTTOM_MASK_HEIGHT_"+index.str(), view->edges[2].height,0,view->height,false);
	gui.addSlider("bottom mask: rotation", "BOTTOM_MASK_ROTATE_"+index.str(), view->edges[2].rotation,-180,180,false);
	
	guiTypePanel * panel2 = addDefaultPanel(name.str()+" LR");
	addDefaultGroup("left mask");
	gui.addSlider("left mask: x", "LEFT_MASK_X_"+index.str(), view->edges[3].x,-view->width,view->width,false);
	gui.addSlider("left mask: y", "LEFT_MASK_Y_"+index.str(), view->edges[3].y,-view->height,view->height,false);
	gui.addSlider("left mask: width", "LEFT_MASK_WIDTH_"+index.str(), view->edges[3].width,0,view->height,false);
	gui.addSlider("left mask: height", "LEFT_MASK_HEIGHT_"+index.str(), view->edges[3].height,0,view->width,false);
	gui.addSlider("left mask: rotation", "LEFT_MASK_ROTATE_"+index.str(), view->edges[3].rotation,-180,180,false);
	addDefaultGroup("right mask");
	gui.addSlider("right mask: x", "RIGHT_MASK_X_"+index.str(), view->edges[1].x,-view->width,view->width,false);
	gui.addSlider("right mask: y", "RIGHT_MASK_Y_"+index.str(), view->edges[1].y,-view->height,view->height,false);
	gui.addSlider("right mask: width", "RIGHT_MASK_WIDTH_"+index.str(), view->edges[1].width,0,view->height,false);
	gui.addSlider("right mask: height", "RIGHT_MASK_HEIGHT_"+index.str(), view->edges[1].height,0,view->width,false);
	gui.addSlider("right mask: rotation", "RIGHT_MASK_ROTATE_"+index.str(), view->edges[1].rotation,-180,180,false);
#endif
			
	gui.loadSettings("settings/guisettings.xml");
	gui.update();	
#endif
	update();
	return view;
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::pushView(){
	if (views.size() <= 0){
		ofLog(OF_LOG_ERROR, "there are no views!");
	}
	pushView(0);
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::pushView( int which ){
	if (views.size() <= 0){
		ofLog(OF_LOG_ERROR, "there are no views!");
		return;
	}
	if (which > views.size() - 1) return;
	views[which]->beginDraw();
	currentView = which;
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::popView(){
	if (currentView != -1) views[currentView]->endDraw();
	currentView = -1;
};


//----------------------------------------------------------------------------
void ofxLabProjectionTools::update(){
#ifdef USE_LAB_GUI
	gui.update();
	
#ifdef USE_SHADERS
	amplifyAmount = gui.getValueF("GUI_AMP");
#endif
	for (int i=0; i<views.size(); i++){
		stringstream index;
		index<<i;
		
#ifndef USE_ADV_MODE		
		views[i]->edges[0].x = views[i]->width;
		views[i]->edges[0].y = views[i]->height;
		views[i]->edges[0].width = views[i]->width;
		views[i]->edges[0].height = gui.getValueF("TOP_MASK_HEIGHT_"+index.str());
		views[i]->edges[0].rotation = 180;
		
		views[i]->edges[1].x = views[i]->width;
		views[i]->edges[1].y = 0;
		views[i]->edges[1].width = views[i]->width;
		views[i]->edges[1].height = gui.getValueF("RIGHT_MASK_HEIGHT_"+index.str());
		views[i]->edges[1].rotation = 90;
		
		views[i]->edges[2].x = 0;
		views[i]->edges[2].y = 0;
		views[i]->edges[2].width = views[i]->width;
		views[i]->edges[2].height = gui.getValueF("BOTTOM_MASK_HEIGHT_"+index.str());
		views[i]->edges[2].rotation = 0;
		
		views[i]->edges[3].x = 0;
		views[i]->edges[3].y = views[i]->width;;
		views[i]->edges[3].width = views[i]->width;
		views[i]->edges[3].height = gui.getValueF("LEFT_MASK_HEIGHT_"+index.str());
		views[i]->edges[3].rotation = -90;
#else
		views[i]->edges[0].x = gui.getValueF("TOP_MASK_X_"+index.str());
		views[i]->edges[0].y = gui.getValueF("TOP_MASK_Y_"+index.str());
		views[i]->edges[0].width = gui.getValueF("TOP_MASK_WIDTH_"+index.str());
		views[i]->edges[0].height = gui.getValueF("TOP_MASK_HEIGHT_"+index.str());
		views[i]->edges[0].rotation = gui.getValueF("TOP_MASK_ROTATE_"+index.str());
		
		views[i]->edges[1].x = gui.getValueF("RIGHT_MASK_X_"+index.str());
		views[i]->edges[1].y = gui.getValueF("RIGHT_MASK_Y_"+index.str());
		views[i]->edges[1].width = gui.getValueF("RIGHT_MASK_WIDTH_"+index.str());
		views[i]->edges[1].height = gui.getValueF("RIGHT_MASK_HEIGHT_"+index.str());
		views[i]->edges[1].rotation = gui.getValueF("RIGHT_MASK_ROTATE_"+index.str());
		
		views[i]->edges[2].x = gui.getValueF("BOTTOM_MASK_X_"+index.str());
		views[i]->edges[2].y = gui.getValueF("BOTTOM_MASK_Y_"+index.str());
		views[i]->edges[2].width = gui.getValueF("BOTTOM_MASK_WIDTH_"+index.str());
		views[i]->edges[2].height = gui.getValueF("BOTTOM_MASK_HEIGHT_"+index.str());
		views[i]->edges[2].rotation = gui.getValueF("BOTTOM_MASK_ROTATE_"+index.str());
		
		views[i]->edges[3].x = gui.getValueF("LEFT_MASK_X_"+index.str());
		views[i]->edges[3].y = views[i]->width;//gui.getValueF("LEFT_MASK_Y_"+index.str());
		views[i]->edges[3].width = views[i]->width;//gui.getValueF("LEFT_MASK_WIDTH_"+index.str());
		views[i]->edges[3].height = gui.getValueF("LEFT_MASK_HEIGHT_"+index.str());
		views[i]->edges[3].rotation = -90;gui.getValueF("LEFT_MASK_ROTATE_"+index.str());
#endif
		
	};
#endif
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::draw(){
#ifdef USE_SHADERS
	if (floorf(amplifyAmount) != 1.0f){
		if (screen.getWidth() == 100){
			screen.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
		};
		screen.clear();
		screen.swapIn();
		ofEnableSmoothing();
	} else {
		//if (screen.getWidth() != 100) screen.allocate(100, 100, GL_RGB);	
	}
#endif
	ofDisableAlphaBlending();
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
	//ofEnableAlphaBlending();
	for (int i=0; i<views.size(); i++){
		draw(i);
	}
#ifdef USE_SHADERS
	if (floorf(amplifyAmount) != 1.0f){
		screen.swapOut();
		amplify.begin();
		screen.bind();
		amplify.setUniform("mult", amplifyAmount);
		amplify.setUniform("src_tex_unit0", 0);
	//views[which]->getTexture().bind();
	//amplify.setUniform("src_tex_unit0", 0);
	//views[which]->getTexture().unbind();
	
		screen.draw(0, 0, screen.getWidth(), screen.getHeight());
		amplify.end();
		screen.unbind();
	}
#endif
	
	string modeString = "";
	
	if (drawMode == LAB_DRAW_MODE_SKEW){
		modeString = "skew";
	} else if ( drawMode == LAB_DRAW_MODE_CROP){
		modeString = "crop";
	} else if ( drawMode == LAB_DRAW_MODE_MASK) {
		modeString = "draw masks";
	} else if ( drawMode == LAB_DRAW_MODE_GUI) {
	} else if (drawMode == LAB_DRAW_MODE_MOVE){
		modeString = "move";
	}
	modeFont.drawString(modeString, 20, ofGetHeight()-40);
	
#ifdef USE_LAB_GUI
	ofSetColor(0xffffff);
	glDisable(GL_DEPTH_TEST);
	if (bDrawGui) gui.draw();
#endif
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::draw( int which ){
	if (which > views.size() -1) return;
	views[which]->draw();
};

/**************************************************************************
 VIEW INFORMATION
 **************************************************************************/

ofPoint ofxLabProjectionTools::getViewResolution( int which ){
	if (which > views.size() -1) return ofPoint();		
	return views[which]->getResolution();
}

ofPoint ofxLabProjectionTools::getRenderPosition( int which ){
	if (which > views.size() -1) return ofPoint();		
	return views[which]->getRenderPosition();
}

/**************************************************************************
 DRAW SKEWABLE CORNERS
 **************************************************************************/

// deprecated
//----------------------------------------------------------------------------
void ofxLabProjectionTools::drawDebug( bool debug){
	bDrawSkew = debug;
	for (int i=0; i<views.size(); i++){
		views[i]->drawSkew(bDrawSkew);
	}
	if (bDrawSkew){
		drawGui(false);
		drawCrop(false);
		drawOverlays(false);
	}
};
// deprecated
//----------------------------------------------------------------------------
void ofxLabProjectionTools::toggleDrawDebug(){
	bDrawSkew = !bDrawSkew;
	for (int i=0; i<views.size(); i++){
		views[i]->drawSkew(bDrawSkew);
	}
	if (bDrawSkew){
		drawGui(false);
		drawCrop(false);
		drawOverlays(false);
	}
}


//----------------------------------------------------------------------------
void ofxLabProjectionTools::drawSkew( bool skew){
	bDrawSkew = skew;
	for (int i=0; i<views.size(); i++){
		views[i]->drawSkew(bDrawSkew);
	}
	if (bDrawSkew){
		drawGui(false);
		drawCrop(false);
		drawOverlays(false);
	}
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::toggleDrawSkew(){
	bDrawSkew = !bDrawSkew;
	for (int i=0; i<views.size(); i++){
		views[i]->drawSkew(bDrawSkew);
	}
	if (bDrawSkew){
		drawGui(false);
		drawCrop(false);
		drawOverlays(false);
	}
}

/**************************************************************************
 DRAW CROP CORNERS
 **************************************************************************/

//----------------------------------------------------------------------------
void ofxLabProjectionTools::drawCrop( bool crop){
	bDrawCropping = crop;
	for (int i=0; i<views.size(); i++){
		views[i]->drawCrop(bDrawCropping);
	}
	if (bDrawCropping){
		drawGui(false);
		drawSkew(false);
		drawOverlays(false);
	}
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::toggleDrawCrop(){
	bDrawCropping = !bDrawCropping;
	for (int i=0; i<views.size(); i++){
		views[i]->drawCrop(bDrawCropping);
	}
	if (bDrawCropping){
		drawGui(false);
		drawSkew(false);
		drawOverlays(false);
	}
}


/**************************************************************************
	 DRAW OVERLAYS
**************************************************************************/

void ofxLabProjectionTools::drawOverlays( bool overlays){
	bDrawOverlays = overlays;
	for (int i=0; i<views.size(); i++){
		views[i]->drawOverlay(bDrawOverlays);
	}
	if (bDrawOverlays){
		drawGui(false);
		drawCrop(false);
		drawSkew(false);
	}
};

/**************************************************************************
 GUI CONTROLS
 **************************************************************************/

#ifdef USE_LAB_GUI
//----------------------------------------------------------------------------
void ofxLabProjectionTools::drawGui( bool gui){
	bDrawGui = gui;
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::toggleDrawGui(){
	bDrawGui = !bDrawGui;
}

//----------------------------------------------------------------------------
ofxLabGui * ofxLabProjectionTools::getGui(){
	return &gui;
};

//----------------------------------------------------------------------------
guiTypePanel * ofxLabProjectionTools::addDefaultPanel( string name ){
	guiTypePanel * panel = gui.addPanel(name, 1, false);		
	panel->setDrawLock( false );	
	panel->setBackgroundColor(174,139,138);
	panel->setBackgroundSelectColor(174,139,138);
	gui.setWhichPanel(name);
	return panel;
}

//----------------------------------------------------------------------------
guiTypeGroup * ofxLabProjectionTools::addDefaultGroup ( string name, bool showText){
	guiTypeGroup * group = gui.addGroup(name);		
	group->setActive(true);
	group->setBackgroundColor(148,129,85);
	group->setBackgroundSelectColor(148,129,85);
	group->seBaseColor(244,136,136);
	group->setShowText(showText);
	return group;
}
#endif

/**************************************************************************
	UTILS
**************************************************************************/

//----------------------------------------------------------------------------
ofxLabProjectedView * ofxLabProjectionTools::getView( int which ){
	return views[which];
};

//----------------------------------------------------------------------------
int ofxLabProjectionTools::getNumViews(){
	return views.size();
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::mousePressed( ofMouseEventArgs & e ){
	int x = e.x;
	int y = e.y;
	
#ifdef USE_LAB_GUI
	if(bDrawGui) gui.mousePressed(x, y, 0);
#else
#endif
	for (int i=views.size()-1; i>=0; i--){
		bool bPressed = views[i]->mousePressed(x,y); 
		if (bPressed) break;
	}
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::mouseReleased( ofMouseEventArgs & e ){
	int x = e.x;
	int y = e.y;
	
#ifdef USE_LAB_GUI
	if(bDrawGui){
		gui.mouseReleased();
		update();
	}
#endif
	for (int i=0; i<views.size(); i++){
		views[i]->mouseReleased();
	}
	saveSettings();
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::mouseDragged( ofMouseEventArgs & e ){
	int x = e.x;
	int y = e.y;
	
#ifdef USE_LAB_GUI
	if(bDrawGui){
		gui.mouseDragged(x, y, 0);
		update();
	}
#else
#endif
	for (int i=0; i<views.size(); i++){
		views[i]->mouseDragged(x,y);
	}
	saveSettings();
};


// WINDOW ----------------------------------------------------------------------------
void ofxLabProjectionTools::windowResized( ofResizeEventArgs & e ){
	screen.allocate(e.width, e.height, GL_RGB, 4);
};

// KEY ----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void ofxLabProjectionTools::enableModes(){
	bEnableDrawmodes = true;
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::disableModes(){
	bEnableDrawmodes = false;
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::toggleEnableModes(){
	bEnableDrawmodes = !bEnableDrawmodes;
};

//----------------------------------------------------------------------------
void ofxLabProjectionTools::keyPressed( ofKeyEventArgs & e ){
	for (int i=0; i<views.size(); i++){
		views[i]->keyPressed(e.key);
	}
	
	if (bEnableDrawmodes){
		if (e.key == 'm'){
			drawMode++;
			if (drawMode > NUM_MODES -1) drawMode = 0;
			
			if (drawMode == LAB_DRAW_MODE_SKEW){
				drawSkew(true);
				drawGui(false);
				drawCrop(false);
				drawOverlays(false);
				ofShowCursor();
			} else if ( drawMode == LAB_DRAW_MODE_CROP){
				drawCrop(true);
				drawGui(true);
				drawSkew(false);
				drawOverlays(false);
				ofShowCursor();
			} else if ( drawMode == LAB_DRAW_MODE_MASK) {
				drawOverlays(true);
				drawGui(false);
				drawSkew(false);
				drawCrop(false);
				ofShowCursor();
				
			} else if ( drawMode == LAB_DRAW_MODE_GUI) {
				drawGui(true);
				drawSkew(false);
				drawCrop(false);
				drawOverlays(false);
				ofShowCursor();
			} else {
				drawGui(false);
				drawSkew(false);
				drawCrop(false);
				drawOverlays(false);
				ofHideCursor();
			};			
			if (drawMode == LAB_DRAW_MODE_MOVE){
				for (int i=0; i<views.size(); i++){
					views[i]->bMovable = true;
				}
				ofShowCursor();
			} else {
				for (int i=0; i<views.size(); i++){
					views[i]->bMovable = false;
				}
			}
		};
	};
	
	saveSettings();
};

// GO THU MODES ----------------------------------------------------------------------------
void ofxLabProjectionTools::nextMode(){
	drawMode++;
	if (drawMode > NUM_MODES -1) drawMode = 0;
	setMode(drawMode);
};	

void ofxLabProjectionTools::setMode( int which ){
	drawMode = which;
	
	if (which == LAB_DRAW_MODE_SKEW){
		drawSkew(true);
		drawGui(false);
		drawCrop(false);
		drawOverlays(false);
		ofShowCursor();
	} else if ( which == LAB_DRAW_MODE_CROP){
		drawCrop(true);
		drawGui(true);
		drawSkew(false);
		drawOverlays(false);
		ofShowCursor();
	} else if ( which == LAB_DRAW_MODE_MASK) {
		drawOverlays(true);
		drawGui(false);
		drawSkew(false);
		drawCrop(false);
		ofShowCursor();
		
	} else if ( which == LAB_DRAW_MODE_GUI) {
		drawGui(true);
		drawSkew(false);
		drawCrop(false);
		drawOverlays(false);
		ofShowCursor();
	} else {
		drawGui(false);
		drawSkew(false);
		drawCrop(false);
		drawOverlays(false);
		ofHideCursor();
	};			
	if (drawMode == LAB_DRAW_MODE_MOVE){
		for (int i=0; i<views.size(); i++){
			views[i]->bMovable = true;
		}
		ofShowCursor();
	} else {
		for (int i=0; i<views.size(); i++){
			views[i]->bMovable = false;
		}
	}
	
};

// SETTINGS ----------------------------------------------------------------------------
void ofxLabProjectionTools::loadSettings(){
	
	bSettingsLoaded = settings.loadFile("projectionsettings.xml");
	if (bSettingsLoaded){
		settings.pushTag("settings");{
			//load views
			cout << "loading "<<settings.getNumTags("view")<<" views"<<endl;
			for (int i=0; i<settings.getNumTags("view"); i++){
				ofxLabProjectedView * view;			
				settings.pushTag("view",i);{
					settings.pushTag("dimensions");{
						float x = settings.getValue("x", 0.0);
						float y = settings.getValue("y", 0.0);
						float width = settings.getValue("width", 0.0);
						float height = settings.getValue("height", 0.0);
						view = addView( x, y, width, height );			
						ofLog(OF_LOG_VERBOSE, "added view at "+ofToString(x)+","+ofToString(y)+":"+ofToString(width)+","+ofToString(height));
					} settings.popTag();
					
					settings.pushTag("skew");{
						for (int j=0; j<settings.getNumTags("point"); j++){		
							float x, y;
							settings.pushTag("point", j);{					
								x = settings.getValue("x", 0.0);
								y = settings.getValue("y", 0.0);
							} settings.popTag();
							view->setSkewPoint(ofPoint(x,y), j);
						}
					} settings.popTag();
					
					settings.pushTag("crop");{
						for (int j=0; j<settings.getNumTags("point"); j++){
							float x, y;
							settings.pushTag("point", j);{					
								x = settings.getValue("x", 0.0);
								y = settings.getValue("y", 0.0);
							} settings.popTag();
							view->setCropPoint(ofPoint(x,y), j);
						}				
					} settings.popTag();
					
					settings.pushTag("overlays");{
						for (int j=0; j<settings.getNumTags("overlay"); j++){
							ofxLabProjectionOverlay * overlay = view->addOverlay();		
							settings.pushTag("overlay", j);{				
								settings.pushTag("position");{
									overlay->pos.x = settings.getValue("x", 0.0);
									overlay->pos.y = settings.getValue("y", 0.0);
								} settings.popTag();
								for (int k=0; k<settings.getNumTags("point"); k++){
									settings.pushTag("point", k);{
										overlay->addPoint(settings.getValue("x", 0.0), settings.getValue("y", 0.0));
									} settings.popTag();
								};
							} settings.popTag();
						}				
					} settings.popTag();
				} settings.popTag();
			};
		} settings.popTag();
	}
};

void ofxLabProjectionTools::saveSettings(){
	settings.clear();
	if (!settings.tagExists("settings")) settings.addTag("settings");
	settings.pushTag("settings");{
		//set views
		for (int i=0; i<views.size(); i++){
			ofxLabProjectedView * view = getView(i);
			
			if (!settings.tagExists("view", i)) settings.addTag("view");
			settings.pushTag("view",i);{
				
				if (!settings.tagExists("dimensions")) settings.addTag("dimensions");
				settings.pushTag("dimensions");{
					settings.setValue("x", view->x);
					settings.setValue("y", view->y);
					settings.setValue("width", view->width);
					settings.setValue("height", view->height);	
				} settings.popTag();
				
				if (!settings.tagExists("skew")) settings.addTag("skew");
				settings.pushTag("skew");{
					for (int j=0; j<4; j++){					
						float x, y;
						if (!settings.tagExists("point", j)) settings.addTag("point");
						settings.pushTag("point", j);{					
							settings.setValue("x", view->getSkewPoint(j).x);
							settings.setValue("y", view->getSkewPoint(j).y);
						} settings.popTag();
					}
				} settings.popTag();
				
				if (!settings.tagExists("crop")) settings.addTag("crop");
				settings.pushTag("crop");{
					for (int j=0; j<4; j++){							
						float x, y;
						if (!settings.tagExists("point", j)) settings.addTag("point");
						settings.pushTag("point", j);{					
							settings.setValue("x", view->getCropPoint(j).x);
							settings.setValue("y", view->getCropPoint(j).y);
						} settings.popTag();
					}				
				} settings.popTag();
				
				if (!settings.tagExists("overlays")) settings.addTag("overlays");
				settings.pushTag("overlays");{
					for (int j=0; j<view->overlays.size(); j++){		
						if (!settings.tagExists("overlay", j)) settings.addTag("overlay");		
						settings.pushTag("overlay", j);{
							if (!settings.tagExists("position")) settings.addTag("position");	
							settings.pushTag("position");{
								settings.setValue("x", view->overlays[j]->pos.x);
								settings.setValue("y", view->overlays[j]->pos.y);
							} settings.popTag();
							
							for (int k=0; k<view->overlays[j]->getNumEdges(); k++){
								if (!settings.tagExists("point", k)) settings.addTag("point");	
								settings.pushTag("point", k);{
									settings.setValue("x", view->overlays[j]->getEdge(k).x);
									settings.setValue("y", view->overlays[j]->getEdge(k).y);
								} settings.popTag();
							}							
						} settings.popTag();
					}				
				} settings.popTag();
			} settings.popTag();
		};
	} settings.popTag();
	settings.saveFile("projectionsettings.xml");
};

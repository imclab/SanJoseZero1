#include <iostream>               // for std::cout
#include <vector>
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(60);	
	
	bUseProjectionTools = true;
	bgColor.r = 0;
	bgColor.g = 0;
	bgColor.b = 0;
	
	//load settings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings/settings.xml");
	settings.pushTag("settings");{
		
		bUseProjectionTools = settings.getValue("bUseProjectionTools", bUseProjectionTools);
		
		//SETUP OSC
				
		//get host + port for sender
		string host = "localhost";
		int port = 12001;
		host = settings.getValue("osc:sender:host",host);
		port = settings.getValue("osc:sender:port",port);
		sender.setup(host,port);
				
		//get port for receiver
		port = 12345;
		port = settings.getValue("osc:receiver:port",port);
		receiver.setup( port );
		
		//get port for calibration receiver
		port = 12010;
		port = settings.getValue("osc:calibrationReceiver:port",port);
		calibrationReceiver.setup( port );
		
		//load transform start + end
		float transformStart	= settings.getValue("transform:start", particleManager.getTransformStart());
		float transformEnd		= settings.getValue("transform:end", 0);//particleManager.getTransformEnd());
				
		particleManager.setTransformStart(transformStart);
		particleManager.setTransformEnd(transformEnd);
				
		//load column spacing
		int numColumns = settings.getValue("columns:numColums",columns.getNumColumns());
		float spacing = settings.getValue("columns:spacing", columns.getSpacing());
		float border = settings.getValue("columns:border", columns.getBorder());
		columns.setNumColumns(numColumns, spacing, border);
		
		//load inputs
		
		float buffer = 100.0f;
		int numInputs = settings.getNumTags("input");
		float increment = (float) (buffer*2.0f + ofGetWidth())/numInputs;
		
		for (int i=0; i<numInputs; i++) {
			BuildingType * type = new BuildingType();			
			settings.pushTag("input", i);
			type->setPosition(settings.getValue("position:x", buffer + increment * (i+1)),settings.getValue("position:y", ofGetHeight()));
			type->setName(settings.getValue("name", "") );
			for (int j=0; j<settings.getNumTags("message"); j++){
				settings.pushTag("message", j);
					type->addMessageString(settings.getValue("messageString", ""));
					type->addColor(settings.getValue("color:r",150), settings.getValue("color:g",150), settings.getValue("color:b",150));
					type->loadModel(ofToDataPath(settings.getValue("image", "")), 4.0f);
				settings.popTag();
			}
			settings.popTag();			
			particleManager.addType(type);
		}	
	} settings.popTag();
	
	particleManager.attachColumns(&columns);
	
	//add listener for particles leaving screen
	ofAddListener(particleManager.particleLeft, this, &testApp::elementLeftScreen);
	
	//try to get calibration settings from the top
	ofxOscMessage getCalibration;
	getCalibration.setAddress( "/pluginplay/getcalibration" );
	sender.sendMessage(getCalibration);
	
	//inital draw mode
	drawMode = LAB_MODE_RENDER;
	
	//particle effects
	trails.setup(&particleManager);
	system = new phyParticleSystem( &particleManager );
	explosionFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
	
	//setup view if not loaded via xml
	if (!projection.bSettingsLoaded){
		projection.addView(0,0,ofGetWidth(), ofGetHeight());
	}
	
	//setup gui
	setupGui();
	
	//setup lights
	ofxMaterialSpecular(120, 120, 120); //how much specular light will be reflect by the surface
	ofxMaterialShininess(100); //how concentrated the reflexion will be (between 0 and 128
	
	//each light will emit white reflexions
	//light1.ambient(50,50,50);
	//light1.diffuse(255, 255, 255);
	//light2.specular(255, 255, 255);
	//light3.specular(255, 255, 255);
	
	float L1DirectionX = .5;
	float L1DirectionY = 1;
	float L1DirectionZ = 0;
	
	light1.directionalLight(255, 255, 255, L1DirectionX, L1DirectionY, L1DirectionZ);
	
	//light2
	float L2ConeAngle = 90;
	float L2Concentration = 60;
	float L2PosX = ofGetWidth()/2.0f;
	float L2PosY = 0;
	float L2PosZ = 500;
	float L2DirectionX = 0;
	float L2DirectionY = 0;
	float L2DirectionZ = 1;
	
	
	light2.spotLight(255, 255, 255, 
					 L2PosX, L2PosY, L2PosZ, 
					 L2DirectionX, L2DirectionY, L2DirectionZ,
					 L2ConeAngle,
					 L2Concentration);
	
	//light3
	float L3PosX = ofGetWidth()/2.0;
	float L3PosY = 0;
	float L3PosZ = 500;
	light3.pointLight(255, 255, 255, L3PosX, L3PosY, L3PosZ);
	
	ofBackground(bgColor.r, bgColor.g, bgColor.b);
	ofHideCursor();
}

//--------------------------------------------------------------
void testApp::update(){
	
	//pause if calibrating particles
	
	if (drawMode != LAB_MODE_P_CALIBRATE ){
		
		// check for waiting messages
		while( receiver.hasWaitingMessages() )
		{
			// get the next message
			ofxOscMessage m;
			receiver.getNextMessage( &m );
			
			bool bFound = false;
			if ( m.getNumArgs() == 0 ) bFound = particleManager.checkAndEmit( m.getAddress() );
			else bFound = particleManager.checkAndEmit( m.getAddress(), m.getArgAsString(0) );
				
			// unrecognized message ;(
			if (!bFound)
			{
				cout<<"not found "<<m.getAddress()<<endl;
			}
		}
		
		particleManager.update();
		trails.update();
		system->update();

		// DRAW EXPLOSION FBO HERE
		// TO AVOID MESSING UP SCREEN MATRICES
		
		explosionFBO.clear();
		explosionFBO.begin();
		ofxLightsOn();
		system->draw();
		ofxLightsOff();
		explosionFBO.end();
	}	
	
	//get values from gui
	ofxLabGui * gui = projection.getGui();
	gui->update();
	
	particleManager.setMinScale(gui->getValueF("SCALE_MIN"));
	particleManager.setMaxScale(gui->getValueF("SCALE_MAX"));
	particleManager.setScaleOut(gui->getValueF("SCALE_OUT"));
	particleManager.setMinSpeed(gui->getValueF("SPEED_MIN"));
	particleManager.setMaxSpeed(gui->getValueF("SPEED_MAX"));
	particleManager.setGroupingTolerance(gui->getValueI("GROUP_TIME"));
	
	bgColor.r = gui->getValueF("BG_COLOR_R");
	bgColor.g = gui->getValueF("BG_COLOR_G");
	bgColor.b = gui->getValueF("BG_COLOR_B");
	
	particleManager.setEmitterDimensions( gui->getValueI("E_WIDTH"), gui->getValueI("E_HEIGHT") );
	
	trails.setDrawMode(gui->getValueI("T_DRAW_MODE"));
	trails.setFade(gui->getValueF("T_FADE"));
	
	bool bNewCalibration = false;
	int numColumns = columns.getNumColumns();
	float spacing = columns.getSpacing();
	float border = columns.getBorder();
	
	// check for calibration messages
	
	while ( calibrationReceiver.hasWaitingMessages() ){
		cout<<"got calibration"<<endl;
		// get the next message
		ofxOscMessage m;
		calibrationReceiver.getNextMessage( &m );
		
		if ( m.getAddress() == "/pluginplay/calibration/numrows"){
			numColumns = m.getArgAsInt32(0);
			bNewCalibration = true;
		} else if ( m.getAddress() == "/pluginplay/calibration/buffers"){
			border = m.getArgAsFloat(0);
			bNewCalibration = true;
		} else if ( m.getAddress() == "/pluginplay/calibration/spacing"){
			spacing = m.getArgAsFloat(0);
			bNewCalibration = true;
		} else if ( m.getAddress() == "/pluginplay/calibration/lightpos"){
			lightPos.x = m.getArgAsFloat(0);
			lightPos.y = m.getArgAsFloat(1);
			lightPos.z = m.getArgAsFloat(2);
			light1.position(lightPos.x, lightPos.y, lightPos.z);
		}
		saveSettings();
	}
	
	if (bNewCalibration)
		columns.setNumColumns(numColumns, spacing, border);
	
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground( bgColor.r, bgColor.g, bgColor.b );
	ofSetColor(0xffffff);
	
	ofxLightsOff();
	
	if (bUseProjectionTools) projection.pushView(0);
	
	//draw particle effects
	glDisable(GL_DEPTH_TEST);
	ofSetColor(255,255,255,150);	
	trails.draw();
	ofSetColor(0xffffff);	
	explosionFBO.draw(0,0);	
	glEnable(GL_DEPTH_TEST);
	
	//draw particles
	particleManager.draw();
	glDisable(GL_DEPTH_TEST);
	ofSetColor( 255, 255, 255 );
	ofxLightsOff();
	if (bUseProjectionTools) projection.popView();
	ofEnableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	
	if ( drawMode == LAB_MODE_P_CALIBRATE ){
		ofEnableAlphaBlending();
		ofSetColor(0,0,0,200);
		ofRect(0, 0, ofGetWidth(), ofGetHeight());
		ofSetColor(0xffffff);		
		ofDisableAlphaBlending();
		
		//calibrate particle scale
		particleManager.drawDebugParticles();
		particleManager.drawTypes();
	}
	projection.draw(bUseProjectionTools);
	
	if ( drawMode == LAB_MODE_CALIBRATE){
		columns.draw();
		particleManager.drawTransformDebug();
		particleManager.drawTypes();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key =='f') ofToggleFullscreen();
	else if (key == 'm'){
		drawMode++;
		if (drawMode > NUM_DRAW_MODES-1){
			drawMode = 0;
			projection.setMode(0);
		}
		
		if (drawMode >= 3){
			projection.setMode(drawMode-2);
		}
		saveSettings();
	}
	
	if (drawMode != LAB_MODE_RENDER){
		ofShowCursor();
	}
	
	if (drawMode != LAB_MODE_P_CALIBRATE){
		if (key == '1'){
			int ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			//particleManager.update();
		} else if (key == '2'){		
			int ran = ofRandom(0, particleManager.getNumTypes()-1);
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
		} else if (key == '3'){		
			int ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
		}else if (key == '4'){		
			int ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
		}else if (key == '5'){		
			int ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
		}
	} else {
		projection.drawGui(true);
	};
	if (key =='p'){
		bUseProjectionTools = !bUseProjectionTools;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	particleManager.mouseDragged(x,y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if (drawMode == LAB_MODE_CALIBRATE){
		particleManager.mousePressedTransform(x, y);
	};
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	particleManager.mouseReleased();
	saveSettings();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	explosionFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);//, 4);
}

//--------------------------------------------------------------
void testApp::elementLeftScreen( ParticleEventArgs & args ){
	ofxOscMessage m;
	m.setAddress(args.address);
	m.addFloatArg( (float) args.loc.x );
	m.addFloatArg( (float) args.vel.y );
	m.addStringArg( args.data );
	m.addFloatArg( (float) args.loc.y );
	sender.sendMessage(m);	
};

//--------------------------------------------------------------
void testApp::saveSettings(){
	//transformEnd
	
	ofxXmlSettings settings;
	settings.loadFile("settings/settings.xml");
	settings.pushTag("settings");{				
		settings.setValue("bUseProjectionTools", bUseProjectionTools);
		
		//transform start + end
		settings.setValue("transform:start", particleManager.getTransformStart());
		settings.setValue("transform:end", particleManager.getTransformEnd());
		
		//column calibrations				
		settings.setValue("columns:numColums",columns.getNumColumns());
		settings.setValue("columns:spacing", columns.getSpacing());
		settings.setValue("columns:border", columns.getBorder());
				
		//input positions
		for (int i=0; i<particleManager.getNumTypes(); i++) {
			BuildingType * type = particleManager.getType(i);
			
			settings.pushTag("input", i);{
				settings.setValue("position:x", type->getPosition().x);
				settings.setValue("position:y", type->getPosition().y);
			} settings.popTag();			
		}
		
	} settings.popTag();	
	settings.saveFile("settings/settings.xml");	
};


//--------------------------------------------------------------
void testApp::setupGui(){
	ofxLabGui * gui = projection.getGui();
	gui->setDimensions(800, 800);
	guiTypePanel * panel = projection.addDefaultPanel("particles");
	projection.addDefaultGroup("settings", true);
	gui->addSlider("minimumScale", "SCALE_MIN", 4.0f, 0.01, 5.0f, false);
	gui->addSlider("maximumScale", "SCALE_MAX", 10.0f, 1.0f, 30.0f, false);
	gui->addSlider("outbound scale", "SCALE_OUT", 4.0f, 0.01, 10.0f, false);
	gui->addSlider("maximum speed", "SPEED_MAX", 10.0f, 1.0f, 100.0f, false);
	gui->addSlider("minumum speed", "SPEED_MIN", 5.0f, .1f, 20.0f, false);
	gui->addSlider("grouping time", "GROUP_TIME", 300, 0, 10000, false);
	
	projection.addDefaultGroup("trails", true);
	gui->addSlider("draw mode", "T_DRAW_MODE", 1, 0, 2, true);
	gui->addSlider("fade amount", "T_FADE", 5., 0., 255., false);
	
	projection.addDefaultPanel("background");
	projection.addDefaultGroup("background", true);
	gui->addSlider("background color red", "BG_COLOR_R", bgColor.r, 0.0, 255.0f, true);
	gui->addSlider("background color green", "BG_COLOR_G", bgColor.g, 0.0, 255.0f, true);
	gui->addSlider("background color blue", "BG_COLOR_B", bgColor.b, 0.0, 255.0f, true);
	
	projection.addDefaultGroup("emitters", true);
	gui->addSlider("width", "E_WIDTH", 100, 1, 500.0f, true);
	gui->addSlider("height", "E_HEIGHT", 400, 1, 600, true);	
	
	projection.loadGuiSettings();
	
	gui->setPanelIndex("particles", 0);
	gui->setPanelIndex("background", 1);
	gui->update();
	gui->setupOscReceiving(3000);
};

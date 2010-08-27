#include <iostream>               // for std::cout
#include <vector>
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(60);	
	
	//load settings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");{
		
		//SETUP OSC
				
		//get host + port for sender
		string host = "localhost";
		int port = 12001;
		port = settings.getValue("osc:sender:port",port);
		sender.setup(host,port);
				
		//get port for receiver
		host = "localhost";
		port = 12345;
		port = settings.getValue("osc:receiver:port",port);
		receiver.setup( port );
		
		//get port for calibration receiver
		host = "localhost";
		port = 12010;
		port = settings.getValue("osc:calibrationReceiver:port",port);
		calibrationReceiver.setup( port );
		
		//load transform start + end
		float transformStart	= settings.getValue("transform:start", particleManager.getTransformStart());
		float transformEnd		= settings.getValue("transform:end", particleManager.getTransformEnd());
		
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
			bFound = particleManager.checkAndEmit(m.getAddress());
				
			// unrecognized message ;(
			if (!bFound)
			{
			}
		}
		
		particleManager.update();
	}
	
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
		}
		saveSettings();
	}
	
	if (bNewCalibration)
		columns.setNumColumns(numColumns, spacing, border);
	
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(0xffffff);
	//draw particles
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	particleManager.draw();
//	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	ofSetColor( 255, 255, 255 );
	
	if ( drawMode == LAB_MODE_CALIBRATE){
		columns.draw();
		particleManager.drawTransformDebug();
		particleManager.drawTypes();
	} else if ( drawMode == LAB_MODE_P_CALIBRATE ){
		//calibrate particle scale
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key =='f') ofToggleFullscreen();
	else if (key == 'm'){
		drawMode++;
		if (drawMode > NUM_DRAW_MODES-1){
			drawMode = 0;
		}
		saveSettings();
	}
	
	if (drawMode != LAB_MODE_P_CALIBRATE){
		
		if (key == '1'){
			int ran = ofRandom(0, particleManager.getNumTypes());
			particleManager.emitRandom(ran);
			//particleManager.update();
		} else if (key == '2'){		
			int ran = ofRandom(0, particleManager.getNumTypes()-1);
			particleManager.emitRandom(ran);
			particleManager.emitRandom(ran+1);
		} else if (key == '3'){		
			int ran = ofRandom(0, particleManager.getNumTypes()-2);
			particleManager.emitRandom(ran);
			particleManager.emitRandom(ran+1);
			particleManager.emitRandom(ran+2);
		}else if (key == '4'){		
			int ran = ofRandom(0, particleManager.getNumTypes()-3);
			particleManager.emitRandom(ran);
			particleManager.emitRandom(ran+1);
			particleManager.emitRandom(ran+2);
			particleManager.emitRandom(ran+3);
		}else if (key == '5'){		
			int ran = ofRandom(0, particleManager.getNumTypes()-4);
			particleManager.emitRandom(ran);
			particleManager.emitRandom(ran+1);
			particleManager.emitRandom(ran+2);
			particleManager.emitRandom(ran+3);
			particleManager.emitRandom(ran+4);
		}
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
	particleManager.windowResized(w,h);
}

//--------------------------------------------------------------
void testApp::elementLeftScreen( ParticleEventArgs & args ){
	ofxOscMessage m;
	m.setAddress(args.address);
	//normalize x coord
	m.addFloatArg( (float) args.loc.x );
	m.addStringArg( args.data );
	sender.sendMessage(m);	
};

//--------------------------------------------------------------
void testApp::saveSettings(){
	//transformEnd
	
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");{		
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
	settings.saveFile("settings.xml");
};


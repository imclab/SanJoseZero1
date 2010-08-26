#include <iostream>               // for std::cout
#include <vector>
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
		
	//load messagestrings from xml
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
		
		int numInputs = settings.getNumTags("input");
		
		//THIS NEEDS TO BE DYNAMIC!
		
		float buffer = 100.0f;
		float increment = (float) (increment*2.0f + ofGetWidth())/numInputs;
		
		//particleManager
		
		for (int i=0; i<numInputs; i++) {	
			BuildingType * type = new BuildingType();
			type->setPosition(buffer + increment * (i+1),0);
			
			if (i <numInputs){			
				settings.pushTag("input", i);
				type->setName(settings.getValue("name", "") );
				for (int j=0; j<settings.getNumTags("message"); j++){
					settings.pushTag("message", j);
						type->addMessageString(settings.getValue("messageString", ""));
						type->loadModel(ofToDataPath(settings.getValue("image", "")), 4.0f);
					settings.popTag();
				}
				settings.popTag();
			};
			
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
}

//--------------------------------------------------------------
void testApp::update(){
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
	}
	
	if (bNewCalibration)
		columns.setNumColumns(numColumns, spacing, border);
	
	particleManager.update();
	
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	
	//no perspective screen
/*	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ofGetWidth(), 0, ofGetHeight(), -100, 2000);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();	
	glTranslatef(ofGetWidth()/2.0f, -ofGetHeight()/2.0f,0);
*/
	ofSetColor(0xffffff);
	//draw particles
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	particleManager.draw();
//	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	ofSetColor( 255, 255, 255 );
	columns.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key =='f') ofToggleFullscreen();
	
	if (key == '1'){
		int ran = ofRandom(0, particleManager.getNumTypes());
		particleManager.emitRandom(ran);
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

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	//particleManager.debug(x,y);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
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


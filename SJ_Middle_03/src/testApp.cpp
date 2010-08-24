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
		
		float buffer = 100.0f;
		float increment = (float) (increment*2.0f + ofGetWidth())/numInputs;
		
		for (int i=0; i<numInputs; i++) {			
			Emitter* e = new Emitter();
			e->setLoc(buffer + increment * (i+1),0);
			e->attachColumns(&columns);
			
			if (i <numInputs){			
				settings.pushTag("input", i);
				e->setName(settings.getValue("name", "") );
				for (int j=0; j<settings.getNumTags("message"); j++){
					settings.pushTag("message", j);
						e->addMessageString(settings.getValue("messageString", ""));
						e->loadModel(ofToDataPath(settings.getValue("image", "")));
					settings.popTag();
				}
				settings.popTag();
			};
			//add listener for particles leaving screen
			ofAddListener(e->particleLeft, this, &testApp::elementLeftScreen);
			
			emitters.push_back(e);
		}	
	} settings.popTag();
	
	//try to get calibration settings from the top
	
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
		for (int i=0; i<emitters.size(); i++){
			if (emitters[i]->checkMessageString(m.getAddress())){
				emitters[i]->emit(emitters[i]->lastFoundString);
				bFound = true;
				break;
			}
		}
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
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
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
	
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->update();
	}
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	
	//no perspective screen
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ofGetWidth(), 0, ofGetHeight(), -100, 2000);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();	
	glTranslatef(ofGetWidth()/2.0f, -ofGetHeight()/2.0f,0);

	ofSetColor(0xffffff);
	//draw particles
	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->draw();
	}
//	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	ofSetColor( 255, 255, 255 );
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key =='f') ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	int ran = ofRandom(0, emitters.size());
	emitters[ran]->emitRandom();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->setLoc(ofGetWidth()/10.0 * (i+1),0);
	}
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


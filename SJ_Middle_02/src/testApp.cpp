#include <iostream>               // for std::cout
#include <vector>
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
//	ofSetFrameRate(60);
		
	//load messagestrings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");
	
	//get host + port for receiver
	string host = "localhost";
	int port = 12345;
	settings.pushTag("receiver");{
		host = settings.getValue("host",host);
//		port = settings.getValue("port",port);
	} settings.popTag();
	// listen on the given port
	receiver.setup( port );
	//	cout << "listening for osc messages on port " << PORT << "\n";
	
	
	int numReceivers = settings.getNumTags("catcher");
		
	// Loops through directories '0' through '9'.
	for (int i=0; i<numReceivers; i++) {
		
		Emitter* e = new Emitter();
		e->setLoc(ofGetWidth()/9.0 * i,0);
		
		if (i <numReceivers){			
			settings.pushTag("catcher", i);
			e->setName(settings.getValue("name", "") );
			for (int j=0; j<settings.getNumTags("message"); j++){
				settings.pushTag("message", j);
					e->addMessageString(settings.getValue("messageString", ""));
					e->loadModel(ofToDataPath(settings.getValue("image", "")));
					cout << "loading "<<settings.getValue("image", "")<<":"<<settings.getValue("messageString", "")<<endl;
				settings.popTag();
			}
			settings.popTag();
		};
		//add listener for particles leaving screen
		ofAddListener(e->particleLeft, this, &testApp::elementLeftScreen);
		
		emitters.push_back(e);
	}
	
	//get host + port for sender
	host = "localhost";
	port = 12001;
	settings.pushTag("sender");{
		host = settings.getValue("host",host);
		port = settings.getValue("port",port);
	} settings.popTag();
	
	settings.popTag();

	
	cout << "setting up sender at "<<host<<","<<port<<endl;
	cout << "there are " << emitters.size() << endl;
	
	//setup OSC
	sender.setup(host, 15000);//port);
	
	
	
	// TEST 3D STUFF
//	testHop.loadModel("hopscotch/hops00.3ds", 20);
//	testHop.setRotation(0, 90, 1, 0, 0);
//	testHop.setRotation(1, 270, 0, 0, 1);
//	testHop.setPosition(ofGetWidth()/2, ofGetHeight()/2, 0);
}

//--------------------------------------------------------------
void testApp::update(){
//	testHop.setRotation(1, 270 + ofGetElapsedTimef() * 60, 0, 1, 1);
//	testHop.setRotation(1, 270 + ofGetElapsedTimef() * 60, 270 + ofGetElapsedTimef() * 60, 0, 1);
//	testHop.setRotation(1, 270 + ofGetElapsedTimef() * 60, 0, 1, 1);
	
	
	// hide old messages
	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		if ( timers[i] < ofGetElapsedTimef() )
			msg_strings[i] = "";
	}
	
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
		
		// unrecognized message: display on screen
		if (!bFound)
		{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for ( int i=0; i<m.getNumArgs(); i++ )
			{
				// get the argument type
				msg_string += m.getArgTypeName( i );
				msg_string += ":";
				// display the argument - make sure we get the right type
				if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
					msg_string += ofToString( m.getArgAsInt32( i ) );
				else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
					msg_string += ofToString( m.getArgAsFloat( i ) );
				else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
					msg_string += m.getArgAsString( i );
				else
					msg_string += "unknown";
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = ( current_msg_string + 1 ) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}
		
	}
	
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
//	glLoadIdentity();	
	glTranslatef(ofGetWidth()/2.0f, -ofGetHeight()/2.0f,0);
	
	//emitterz
	// draw a quad that fills the whole screen
	glBegin(GL_QUADS);{
		glColor3f( 255.0f, 255.0f, 255.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( ofGetWidth(), 0.0f, 0.0f );
		glColor3f( 0.0f, 0.3f, 0.3f );
		glVertex3f( ofGetWidth(), ofGetHeight(), 0.0f );
		glVertex3f( 0.0f, ofGetHeight(), 0.0f );
	} glEnd();
	
//	glEnable(GL_DEPTH_TEST);
//	ofSetColor(255,255,255,255);
//	ofEnableAlphaBlending();
//	ofPushMatrix();{
//		ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
//		//ofRotateY(mouseX - ofGetWidth());
//		ofTranslate(-ofGetWidth()/2.0, -ofGetHeight()/2.0);
//		//draw particles
//		testHop.draw();
//
//	} ofPopMatrix();
//	testHop.draw();	
//	ofDisableAlphaBlending();
//	glDisable(GL_DEPTH_TEST);
	
	ofSetColor(0xffffff);
//	ofEnableAlphaBlending();
	//draw particles
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->draw();
	}
//	ofDisableAlphaBlending();
	
	string buf;
	buf = "listening for osc messages on port" + ofToString( 12000 );
	ofDrawBitmapString( buf, 10, 20 );
	ofSetColor( 255, 255, 255 );
	
	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		ofDrawBitmapString( msg_strings[i], 10, 40+15*i );
	}
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
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->setLoc(ofGetWidth()/9.0 * i,0);
	}
}

//--------------------------------------------------------------
void testApp::elementLeftScreen( ParticleEventArgs & args ){
	ofxOscMessage m;
	m.setAddress(args.address);
	//normalize x coord
	m.addFloatArg( (float) args.loc.x );
	sender.sendMessage(m);	
};


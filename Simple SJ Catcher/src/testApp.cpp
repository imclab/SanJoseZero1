#include <iostream>               // for std::cout
#include <vector>
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup( PORT );
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	ofxDirList DIR;
	
	//load messagestrings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");
	
	int numReceivers = settings.getNumTags("receiver");
	
	// Loops through directories '0' through '9'.
	for (int i=0; i<10; i++) {
		
		Emitter * e = new Emitter();
		e->setLoc(ofGetWidth()/9.0 * i,0);
		int numImgs = DIR.listDir(ofToString(i));
		if (numImgs > 0){
			for (int j=0; j<numImgs; j++) e->loadImage(DIR.getPath(j));		
		}
		
		if (i <numReceivers){			
			settings.pushTag("receiver", i);
			for (int j=0; j<settings.getNumTags("messageString"); j++){
				e->addMessageString(settings.getValue("messageString", "", j));
				cout<< "adding "<<settings.getValue("messageString", "", j)<<" to "<< i<<endl;
			}
			settings.popTag();
		};
		
		emitters.push_back(e);
	}	
	
	settings.popTag();
	
	/*
	emitters[0]->addMessageString("/pluginplay/picnictable");
	emitters[1]->addMessageString("/pluginplay/megaphone");
	emitters[2]->addMessageString("/pluginplay/birdfeeder");
	emitters[3]->addMessageString("/pluginplay/hopscotch");
	emitters[4]->addMessageString("/pluginplay/stoplight");
	emitters[5]->addMessageString("/pluginplay/playground");
	emitters[6]->addMessageString("/pluginplay/twitter");
	emitters[7]->addMessageString("/pluginplay/flickr");
	emitters[8]->addMessageString("/pluginplay/foursquare");
	emitters[8]->addMessageString("/pluginplay/foursquare/nearby");
	 */
	//emitters[9]->addMessageString ("/pluginplay/");
}

//--------------------------------------------------------------
void testApp::update(){
	
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
	//emitterz
	// draw a quad that fills the whole screen
	glBegin(GL_QUADS);{
		glColor3f( 0.0f, 0.3f, 0.3f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( ofGetWidth(), 0.0f, 0.0f );
		glColor3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( ofGetWidth(), ofGetHeight(), 0.0f );
		glVertex3f( 0.0f, ofGetHeight(), 0.0f );
	} glEnd();
	
	ofSetColor(0xffffff);
	
	//draw particles
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->draw();
	}
	
	string buf;
	buf = "listening for osc messages on port" + ofToString( PORT );
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
	
}



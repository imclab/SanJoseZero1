#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
		}settings.popTag();
	};
	
	cout << "setting up sender at "<<host<<","<<port<<endl;
	
	//setup OSC
	sender.setup(host, port);
	ofSetFrameRate(30);
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetElapsedTimeMillis() - lastSent > SEARCH_TIME){
		lastSent = ofGetElapsedTimeMillis();
		sendSignal();
	} else {
		bSent = false;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (bSent){
		ofBackground(0,150,0);
	} else {
		ofBackground(0,0,0);
	}
}

//--------------------------------------------------------------
void testApp::sendSignal(){
	bSent = true;
	ofxOscMessage m;
	m.setAddress("/pluginplay/signal_00");
	m.addIntArg(0);
	sender.sendMessage(m);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	sendSignal();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


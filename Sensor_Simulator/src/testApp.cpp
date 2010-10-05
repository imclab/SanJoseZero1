#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	int port = settings.getValue("osc:port",12345);
	string host = settings.getValue("osc:host","localhost");
	message = settings.getValue("message", "");
	sender.setup(host, port);
	lastTimeSent = ofGetElapsedTimeMillis();
	delay = ofRandom(100, 10000);
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetElapsedTimeMillis() - lastTimeSent > delay){
		ofxOscMessage m;
		m.setAddress(message);
		m.addFloatArg(ofRandomf()*100.0f);
		sender.sendMessage(m);
		lastTimeSent = ofGetElapsedTimeMillis();
		delay = ofRandom(100, 10000);
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofDrawBitmapString(message, 20,20);
	ofDrawBitmapString(ofToString(lastTimeSent)+":"+ofToString(ofGetElapsedTimeMillis()), 20,40);
	ofDrawBitmapString(ofToString(delay), 20,60);
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

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


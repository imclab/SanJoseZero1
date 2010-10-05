#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	settings.listDir(ofToDataPath("images/"));
	cout<<settings.getPath(0)<<":"<<settings.getName(0)<<endl;
	image1.loadImage(settings.getPath(0));
	image2.loadImage(settings.getPath(1));
	lastLoaded = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetElapsedTimeMillis() - lastLoaded > 2000){
		settings.listDir(ofToDataPath("images/"));
		if(settings.listDir(ofToDataPath("images/"))){
			image1.loadImage(settings.getPath(0));
			image2.loadImage(settings.getPath(1));
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	image1.draw(0, 0);
	image2.draw(0,ofGetHeight()/2.0f);
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


#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofxLabGui * gui = projection.getGui();
	guiTypePanel * panel = projection.addDefaultPanel("particles");
	projection.addDefaultGroup("settings", true);
	gui->addSlider("minimumScale", "SCALE_MIN", 4.0f, 0.01, 5.0f, false);
	gui->addSlider("maximumScale", "SCALE_MAX", 10.0f, 1.0f, 30.0f, false);
	projection.loadGuiSettings();
	
	gui->setPanelIndex("particles", 0);
	gui->update();
	projection.drawGui(true);
	
	string host = "localhost";
	int port = 3000;
	
	ofxXmlSettings settings;
	bool loaded = settings.loadFile("settings.xml");
	if (loaded){
		host = settings.getValue("settings:osc:host", host);
		port = settings.getValue("settings:osc:port", port);
	}		
	
	gui->setupOscSending(host, port);
}

//--------------------------------------------------------------
void testApp::update(){
	projection.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	projection.draw();
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

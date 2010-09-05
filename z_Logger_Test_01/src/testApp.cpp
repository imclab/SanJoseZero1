#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofAddListener(httpUtils.newResponseEvent,this,&testApp::newResponse);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofDrawBitmapString(responseString,20,20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	ofxHttpForm form;
	form.action = POST_URL;
	form.method = OFX_HTTP_POST;
	int index = 0;
	for (int i=0; i<10; i++){
		float ranBuildings = 5;//ofRandom(1, 10);
		for (int j=0; j<ranBuildings; j++){
			form.addFormField("buildings["+ofToString(index)+"][type]", "twitter");
			form.addFormField("buildings["+ofToString(index)+"][data]", "Just had a meeting about my 401K and stock options #dollarbills");
			form.addFormField("buildings["+ofToString(index)+"][row]", ofToString(i));
			form.addFormField("buildings["+ofToString(index)+"][index]", ofToString(j));
			index++;
		}
	}
	
	httpUtils.addForm(form);
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

//--------------------------------------------------------------
void testApp::newResponse(ofxHttpResponse & response){
	responseString = ofToString(response.status) + ": " + response.responseBody;
}
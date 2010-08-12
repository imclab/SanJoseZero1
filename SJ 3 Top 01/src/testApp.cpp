#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup( PORT );
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(120);
	
	//load messagestrings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");
	
	int numReceivers = settings.getNumTags("receiver");
	
	// Loops through directories '0' through '9'.
	for (int i=0; i<numReceivers; i++) {
		
		Emitter * e = new Emitter();
		e->setLoc(ofGetWidth()/9.0 * i,0);
		
		if (i <numReceivers){			
			settings.pushTag("receiver", i);
			e->setName(settings.getValue("name", "") );
			for (int j=0; j<settings.getNumTags("message"); j++){
				settings.pushTag("message", j);
				e->addMessageString(settings.getValue("messageString", ""));
				e->loadModel(ofToDataPath(settings.getValue("image", "")), 2.5);
				cout << "loading "<<settings.getValue("image", "")<<":"<<settings.getValue("messageString", "")<<endl;
				settings.popTag();
			}
			settings.popTag();
		};
		
		emitters.push_back(e);
	}
	settings.popTag();
	
	//lights
	ofxMaterialSpecular(120, 120, 120); //how much specular light will be reflect by the surface
	ofxMaterialShininess(128); //how concentrated the reflexion will be (between 0 and 128
	
	//each light will emit white reflexions
	//light1.ambient(50,50,50);
	//light1.diffuse(255, 255, 255);
	//light2.specular(255, 255, 255);
	//light3.specular(255, 255, 255);
	
	float L1DirectionX = 1;
	float L1DirectionY = 0;
	float L1DirectionZ = 0;
	
	light1.directionalLight(200, 200, 200, L1DirectionX, L1DirectionY, L1DirectionZ);
	
	//light2
	float L2ConeAngle = 90;
	float L2Concentration = 60;
	float L2PosX = ofGetWidth()/2.0f;
	float L2PosY = ofGetHeight()/2.0;
	float L2PosZ = 500;
	float L2DirectionX = 0;
	float L2DirectionY = 0;
	float L2DirectionZ = -1;
	
	
	 light2.spotLight(255, 255, 255, 
					 L2PosX, L2PosY, L2PosZ, 
					 L2DirectionX, L2DirectionY, L2DirectionZ,
					 L2ConeAngle,
					 L2Concentration);
	
	//light3
	float L3PosX = ofGetWidth();
	float L3PosY = 0;
	float L3PosZ = 500;
	light3.pointLight(255, 255, 255, L3PosX, L3PosY, L3PosZ);
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
		
		// unrecognized message: display on screen
		if (!bFound)
		{
			// unrecognized message
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
	glTranslatef(ofGetWidth()/2.0f, -ofGetHeight()/2.0f,0);
	
	// draw a quad that fills the whole screen
	glBegin(GL_QUADS);{
		glColor3f( .5f, .5f, .75f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( ofGetWidth(), 0.0f, 0.0f );
		glColor3f( 0.0f, 0.3f, 0.3f );
		glVertex3f( ofGetWidth(), ofGetHeight(), 0.0f );
		glVertex3f( 0.0f, ofGetHeight(), 0.0f );
	} glEnd();
	
	ofSetColor(0x333333);
	ofRect(0,0,ofGetWidth(), emitters[0]->originalCeiling);
	
	ofxLightsOn();
	glEnable(GL_DEPTH_TEST);
	
	ofSetColor(0xffffff);
	ofEnableAlphaBlending();
	//draw particles
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->draw();
	}
	ofDisableAlphaBlending();
	
	glDisable(GL_DEPTH_TEST);
	
	ofxLightsOff();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key =='f') ofToggleFullscreen();
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


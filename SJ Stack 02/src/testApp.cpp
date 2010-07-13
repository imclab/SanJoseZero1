#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup( PORT );
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(120);
	
	particleManager = new Emitter();
	
	//lights
	ofxMaterialSpecular(120, 120, 120); //how much specular light will be reflect by the surface
	ofxMaterialShininess(128); //how concentrated the reflexion will be (between 0 and 128
	
	//each light will emit white reflexions
	//light1.ambient(50,50,50);
	//light1.diffuse(255, 255, 255);
	//light2.specular(255, 255, 255);
	//light3.specular(255, 255, 255);
	
	float L1DirectionX = .5;
	float L1DirectionY = 1;
	float L1DirectionZ = 0;
	
	light1.directionalLight(255, 255, 255, L1DirectionX, L1DirectionY, L1DirectionZ);
	
	//light2
	float L2ConeAngle = 90;
	float L2Concentration = 60;
	float L2PosX = ofGetWidth()/2.0f;
	float L2PosY = 0;
	float L2PosZ = 500;
	float L2DirectionX = 0;
	float L2DirectionY = 0;
	float L2DirectionZ = 1;
	
	
	 light2.spotLight(255, 255, 255, 
					 L2PosX, L2PosY, L2PosZ, 
					 L2DirectionX, L2DirectionY, L2DirectionZ,
					 L2ConeAngle,
					 L2Concentration);
	
	//light3
	float L3PosX = ofGetWidth()/2.0;
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
		
		bFound = particleManager->checkMessageString(m.getAddress(), m.getArgAsInt32(0));
		
		if (!bFound)
		{
			cout << "did not recognize: "<<m.getAddress()<<endl;
			// unrecognized message
		}
	};
	
	particleManager->update();
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
};

//--------------------------------------------------------------
void testApp::draw(){
	//no perspective screen
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ofGetWidth(), 0, ofGetHeight(), -100, 2000);
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(ofGetWidth()/2.0f, -ofGetHeight()/2.0f,0);
	*/
	
	//light perspective screen
	int w, h;
	
	w = ofGetWidth();
	h = ofGetHeight();
	
	float halfFov, theTan, screenFov, aspect;
	screenFov 		= 40.0f; //adjust this to alter the perspective
	
	//do other normal set up
	
	float eyeX 		= (float)w / 2.0;
	float eyeY 		= (float)h / 2.0;
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	float dist 		= eyeY / theTan;
	float nearDist 	= dist / 50.0;	// near / far clip plane
	float farDist 	= dist * 50.0;
	aspect 			= (float)w/(float)h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(screenFov, aspect, nearDist, farDist);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, dist, eyeX, eyeY, 0.0, 0.0, 1.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.
	
	ofxLightsOn();
	
	// draw background gradient
	
	glBegin(GL_QUADS);{
		glColor3f( .5f, .5f, .75f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( ofGetWidth(), 0.0f, 0.0f );
		glColor3f( 0.0f, 0.3f, 0.3f );
		glVertex3f( ofGetWidth(), ofGetHeight(), 0.0f );
		glVertex3f( 0.0f, ofGetHeight(), 0.0f );
	} glEnd();
		
	glEnable(GL_DEPTH_TEST);
	ofSetColor(0xffffff);
	ofEnableAlphaBlending();
	ofPushMatrix();{
		ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
		//ofRotateY(60);
		ofTranslate(-ofGetWidth()/2.0, -ofGetHeight()/2.0);
		//draw particles
		particleManager->draw();	
	} ofPopMatrix();
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
	particleManager->emitRandom();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


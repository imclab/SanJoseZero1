#include "testApp.h"

ofImage ceilingImage;

//--------------------------------------------------------------
void testApp::setup(){
	//load settings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");{
		
		//get port for receiver
		int rPort = 12345;
		rPort = settings.getValue("osc:receiver:port",12001);
		receiver.setup( rPort );
		
		int sPort = 12005;
		string sHost = "localhost";
		sPort = settings.getValue("osc:loggerSender:port",sPort);
		sHost = settings.getValue("osc:loggerSender:host",sHost);		
		loggerSender.setup(sHost, sPort);
		
		int cPort = 12010;
		string cHost = "localhost";
		cPort = settings.getValue("osc:calibrationSender:port",sPort);
		cHost = settings.getValue("osc:calibrationSender:host",sHost);
		calibrationSender.setup(cHost, cPort);
		
	} settings.popTag();
	
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
	
	// add listener to send to the logger app
	ofAddListener(particleManager->rowComplete,this,&testApp::rowIsComplete);	
	
	// PLACEHOLDER: send calibration: # of rows
	ofxOscMessage rowMessage;
	rowMessage.setAddress("/pluginplay/calibration/numrows");
	rowMessage.addIntArg((int) NUMBER_OF_ROWS);
	calibrationSender.sendMessage(rowMessage);	
	
	ofxOscMessage buffermessage;
	buffermessage.setAddress("/pluginplay/calibration/buffers");
	buffermessage.addFloatArg( ROW_BUFFER);
	calibrationSender.sendMessage(buffermessage);
	
	ofxOscMessage spacerMessage;
	spacerMessage.setAddress("/pluginplay/calibration/spacing");
	spacerMessage.addFloatArg( ROW_SPACING );
	calibrationSender.sendMessage(spacerMessage);
	
	//shaders
	depthShader.setup("shaders/depthShader");
	shadowShader.setup("shaders/shadow");	
	
	//fbo
	//fbo.allocate(1024, 1024, GL_RGBA, 4);
	//fbo.allocate(2048, 2048, true);//using older FBO version for the time being
	fbo.allocate(1024, 1024, true);//using older FBO version for the time being
	fbo.clear(1.0, 1.0, 1.0, 1.f);
	
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	
	fCounter = 0.0;
	ceilingImage.loadImage("hop_3.png");
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
		
		bFound = particleManager->checkMessageString(m.getAddress(), m.getArgAsFloat(0));
		
		if (!bFound)
		{
			cout << "did not recognize: "<<m.getAddress()<<endl;
			// unrecognized message
		}
	};
	
	
	//LARS-temporary	
	/*if (fCounter/ofGetFrameRate() > 0.2){		
		particleManager->emitRandom();
		fCounter = 0.0;
	}
	fCounter++;*/
	//temporary, just to create blocks
	
	particleManager->update();
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
};

//--------------------------------------------------------------
void testApp::draw(){	
	
	//set up screen
		int w, h;		
		float halfFov, theTan, screenFov, aspect;
	
		w = ofGetWidth();
		h = ofGetHeight();		
		screenFov 		= 60.0f; //adjust this to alter the perspective
		float eyeX 		= (float)w / 2.0;
		float eyeY 		= (float)h / 2.0;
		halfFov 		= PI * screenFov / 360.0;
		theTan 			= tanf(halfFov);
		float dist 		= eyeY / theTan;
	float nearDist		= 150; //dist / 50.0;	// near / far clip plane
	float farDist		= dist * 50.0;
		aspect 			= (float)w/(float)h;
	
	
	//ofxVec3f lightPos(mouseX, sin(mouseY * 0.001) * -2000.0, cos(mouseY * 0.01)*2000.0-100.0);
	//ofxVec3f lightPos(1010, -760, 1150);
	ofxVec3f lightPos(eyeX + sin(ofGetElapsedTimef()*0.05)*800.0, -960, 1300);
	ofxVec3f targetPos(eyeX, eyeX, 405);
	
	
	ofxOscMessage lightpos;
	lightpos.setAddress("/pluginplay/calibration/lightpos");
	lightpos.addFloatArg( lightPos.x );
	lightpos.addFloatArg( lightPos.y );
	lightpos.addFloatArg( lightPos.z );
	calibrationSender.sendMessage(lightpos);
	
	//draw shadow map
	//fbo.clear(1.0, 1.0, 1.0, 1.0);
	fbo.begin();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(screenFov, aspect, nearDist, farDist);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(lightPos.x, lightPos.y, lightPos.z,
				  targetPos.x, targetPos.y, targetPos.z,
				  0.0, 1.0, 0.0);
		
		glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
		glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.
	
	//draw things
	
	glEnable(GL_DEPTH_TEST);
	
	//depthShader.begin();
	ofPushMatrix();{
		ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
		if(bDragging) ofRotateY(mouseX - ofGetWidth()/2.0);
		ofTranslate(-ofGetWidth()/2.0, -ofGetHeight()/2.0);
		
		//draw particles
		particleManager->draw();
		
		//grab matrices to pass to shadow shader
		grabMatrices(GL_TEXTURE7);
		
	} ofPopMatrix();
	//depthShader.end();
	
	
	//copy depth buffer data to depthTexture.
	//was acting weird with the recent ofxFBO, so I switched to an older ofxFBO
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					 0, 0, fbo.getWidth(), fbo.getHeight(),0);
	
	ofDisableAlphaBlending();
	
	fbo.end();
	
	//draw geometry
	//set camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(screenFov, aspect, nearDist, farDist);	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY-200, 1100,
			  eyeX, eyeY, 600.0,
			  0.0, 1.0, 0.0);	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
	glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.
	
	
	shadowShader.begin();	
	shadowShader.setUniform("sampStep", 1.f/float(fbo.getWidth()), 1.f/float(fbo.getHeight()));	
	shadowShader.setUniform("lPos", lightPos.x, lightPos.y, lightPos.z);	
	shadowShader.setUniform("ShadowMap", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTexture);	
	shadowShader.setUniform("inTex", 0);
	glActiveTexture(GL_TEXTURE0);	
	
	glEnable(GL_DEPTH_TEST);
	ofSetColor(0xffffff);
	//ofEnableAlphaBlending();
	
	ofxLightsOn();	
	ofPushMatrix();{

		ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
		if(bDragging) ofRotateY(mouseX - ofGetWidth()/2.0);
		ofTranslate(-ofGetWidth()/2.0, -ofGetHeight()/2.0);
			
		//get modelViewMatrix and invert it
		float modelView[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
		ofxMatrix4x4 invModelView(modelView);
		invModelView.makeInvertOf(invModelView);		
		//pass the inverse of modelViewMatrix to the shader
		glUniformMatrix4fv(glGetUniformLocationARB(shadowShader.shader, "invMat"),
						   1, GL_FALSE, invModelView.getPtr());
			
		//draw particles
		particleManager->draw();	
		
	} ofPopMatrix();	
	ofxLightsOff();
		
	shadowShader.end();			
	
	
	
	//draw and write diagnostics
	setOrthographicProjection(0, ofGetWidth(),
							  0, ofGetHeight(),
							  -1000, 1000);	
	glDisable(GL_DEPTH_TEST);
	
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);	
	fbo.bind();
	//glBindTexture(GL_TEXTURE_2D, depthTexture);
	drawTex(20, 20, 200, 200);
	
	ofSetColor(255, 0, 0);
	string fpsString = "light position  " + ofToString(lightPos.x, 2) + ", " + ofToString(lightPos.y, 2) + ", " + ofToString(lightPos.z, 2);
	ofDrawBitmapString(fpsString,  20, 20);
	fpsString = "target position  " + ofToString(targetPos.x,2)+", "+ofToString(targetPos.y,2)+", "+ofToString(targetPos.z,2);
	ofDrawBitmapString(fpsString,  20, 40);
	
}

//--------------------------------------------------------------
void testApp::rowIsComplete( BuildingRow * &completedRow ){
	
	ofxOscMessage newRowMessage;
	newRowMessage.setAddress("/pluginplay/newrow");
	
	for (int i=0; i<completedRow->stacks.size(); i++){
		for (int j=0; j<completedRow->stacks[i]->buildings.size(); j++){
			Building * b = completedRow->stacks[i]->buildings[j];
			
			newRowMessage.addStringArg(b->getType());
			newRowMessage.addStringArg(b->getData());
			newRowMessage.addStringArg(ofToString(i));
			newRowMessage.addStringArg(ofToString(j));
			//form.addFormField("buildings["+ofToString(index)+"][type]", b->getType());
			//form.addFormField("buildings["+ofToString(index)+"][data]", "");//b->getData());
			//form.addFormField("buildings["+ofToString(index)+"][row]", ofToString(i));
			//form.addFormField("buildings["+ofToString(index)+"][index]", ofToString(j));
		}
	}
	loggerSender.sendMessage(newRowMessage);	
	
};

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
	//bDragging = true;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	particleManager->emitRandom();
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	bDragging = false;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	particleManager->windowResized();
}
//--------------------------------------------------------------
void testApp::grabMatrices(GLenum gl_textureTarget){
	static double modelView[16];
	static double projection[16];
	
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] = { 
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};
	
	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView); 
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(gl_textureTarget);
	glLoadIdentity();
	glLoadMatrixd(bias);
	
	// concatating all matrices into one. 
	glMultMatrixd (projection);
	glMultMatrixd (modelView);	
	
	// Go back to normal matrix mode 
	glMatrixMode(GL_MODELVIEW);
}

//--------------------------------------------------------------
void testApp::setOrthographicProjection(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
	
	glMatrixMode(GL_PROJECTION);// switch to projection mode	
	glPushMatrix();// save previous matrix which contains the settings for the perspective projection	
	glLoadIdentity();// reset matrix	
	glOrtho( xMin, xMax, yMin, yMax, zMin, zMax);// set a 2D orthographic projection//gluOrtho2D(0, w, 0, h);	
	glScalef(1, -1, 1);	// invert the y axis, down is positive
	glTranslatef(0, -yMax, 0);// mover the origin from the bottom left corner to the upper left corner
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
}
//--------------------------------------------------------------
void testApp::drawTex(int x, int y, int w, int h){
	glNormal3f(0.0, 0.0, 1.0);
	ofSetColor(255,255,255);
	glBegin(GL_QUADS);	
	
	//glTexCoord2f(0.0, 1.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(x, y);	
	
	//glTexCoord2f(1.0, 1.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(x+w, y);	
	
	//glTexCoord2f(1.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(x+w, y+h);	
	
	//glTexCoord2f(0.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(x, y+h);	
	
	glEnd();		
}


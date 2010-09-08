#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(120);
	ofDisableArbTex();
	
	//lights
	ofxMaterialSpecular(120, 120, 120); //how much specular light will be reflect by the surface
	ofxMaterialShininess(128); //how concentrated the reflexion will be (between 0 and 128
	
	//each light will emit white reflexions
	//light1.ambient(50,50,50);
	light1.diffuse(255, 255, 255);
	light2.diffuse(255, 255, 255);
	light3.diffuse(255, 255, 255);
	
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
	
	//set screen vars
	screenWidth = ofGetWidth();
	screenHeight = ofGetHeight();		
	screenFov 		= 60.0f; //adjust this to alter the perspective
	eyeX 		= (float)screenWidth / 2.0;
	eyeY 		= (float)screenHeight / 2.0;
	targetPos.set(eyeX, eyeX, 205);
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	dist 		= eyeY / theTan;
	nearDist		= 150; //dist / 50.0;	// near / far clip plane
	farDist		= dist * 50.0;
	aspect 			= (float)screenWidth/(float)screenHeight;
	
	//shaders
	depthShader.setup("shaders/depthShader");
	shadowShader.setup("shaders/shadow");	
	simpleTex.setup("shaders/facingRatio");	
	simpleTex.setUniform("inTex", 0);
	
	//fbo
	//fbo.allocate(2048, 2048, GL_RGBA, 4);
	fbo.allocate(2048, 2048, true);//using older FBO version for the time being
	//fbo.allocate(1024, 1024, true);//using older FBO version for the time being
	fbo.clear(1.0, 1.0, 1.0, 1.f);
	
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	
	//ceiling mesh	
	numCurves = NUMBER_OF_ROWS;//number of stacks per row + 2(border)
	numCVs = 18;
	numSubdivisions = 25;
	
	uIncrement = .02; //this gets added to advance mesh verts and stack positions. added to uVal per frame
		
	fCounter = 0.0;
	wireFrame = false;
	bDrawCurves = false;
	bDragging = false;
	fullScreenStarted = ofGetElapsedTimeMillis();
	fullScreenWaitTime = 5000;
	
	//setup particle manager
	particleManager = new Emitter(ceiling);
	particleManager->setAdvanceCeilingBool(&advanceCeiling);
	
	//initial settings
	lightPosition.x = 796;
	lightPosition.y = ofGetHeight()*.5;
	lightPosition.z = ofGetHeight()*1.5;
	bLightSourceChanged = true;
	string ceilingImagePath = "ceiling.jpg";
	
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
		cPort = settings.getValue("osc:calibrationSender:port",cPort);
		cHost = settings.getValue("osc:calibrationSender:host",cHost);
		calibrationSender.setup(cHost, cPort);
		
	} settings.popTag();
	
	// send calibration: # of rows
	ofxOscMessage rowMessage;
	rowMessage.setAddress("/pluginplay/calibration/numrows");
	rowMessage.addIntArg((int) NUMBER_OF_ROWS);
	calibrationSender.sendMessage(rowMessage);	
	
	ofxOscMessage spacerMessage;
	spacerMessage.setAddress("/pluginplay/calibration/buffers");
	spacerMessage.addFloatArg( ROW_BUFFER);
	calibrationSender.sendMessage(spacerMessage);
	
	ofxOscMessage spaceMessage;
	spaceMessage.setAddress("/pluginplay/calibration/spacing");
	spaceMessage.addFloatArg( ROW_SPACING);
	calibrationSender.sendMessage(spaceMessage);
	
	// add listener to send to the logger app
	ofAddListener(particleManager->rowComplete,this,&testApp::rowIsComplete);
	
	ceilingImage.loadImage(ceilingImagePath);
	setupConveyorMesh();
	//scaleConveyorY(.5f);
	cout<<"all set up"<<endl;
	bInited = true;
}

//--------------------------------------------------------------
void testApp::update(){
	//yikes, gross hack
	if (ofGetElapsedTimeMillis() - fullScreenStarted < fullScreenWaitTime) return;
	
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
				
		bool bFound = false;
		
		bFound = particleManager->checkMessageString(m.getAddress(), m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsString(2));
		
		if (!bFound)
		{
			cout << "did not recognize: "<<m.getAddress()<<endl;
			// unrecognized message
		}
	};	
	
	particleManager->update();
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
	
	// ROTATING!
	
	if(advanceCeiling){
		//advance all the mesh nodes
		for(int i=0;i<meshNodes.size();i++){
			meshNodes[i].uPos += uIncrement;
		}
		//delete any stacks that have a uPos > 1.0
		for(int i=refVerts.size()-1; i>=0;i--){
			if (meshNodes[refVerts[i]].uPos >= 1.0) {
				refVerts.erase(refVerts.begin()+i);
				stacks.erase(stacks.begin()+i);
			}
		}
		//wrap meshnodes that have grown bigger then 1.0 
		for(int i=0;i<meshNodes.size();i++){
			if(meshNodes[i].uPos >1.0)	meshNodes[i].uPos -= 1.0;
			if(meshNodes[i].uPos <0.0)	meshNodes[i].uPos += 1.0;
			meshNodes[i].update();
		}
		
		
		//set stack position and rotation
		for(int i=refVerts.size()-1; i>=0;i--){
			stacks[i].setPosition(vertices[refVerts[i]].pos);
			//find rotation degree
			ofxVec3f upVec(0,1,0);
			stacks[i].angle = upVec.angle(vertices[refVerts[i]].norm) - 90;				
			stacks[i].rotAxis = ofxVec3f(1,0,0);//if we deform the surface more we need to 
												//set axis perpendicular to normal
		}
	}
	//sweetSpot.uPos = float(mouseY)/float(ofGetHeight());
	for(int i=0;i<sweetSpot.size();i++){
		sweetSpot[i].update();
	}
	
	float oldRowBuffer = ROW_BUFFER;
	float oldRowSpacing = ROW_SPACING;	
	
	ROW_BUFFER = vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x;
	ROW_SPACING = vertices[getVertPointer(1, sweetSpot[2].uPos)].pos.x - vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x ;
	
	particleManager->setCeiling(vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.y);
	
	//send calibration if new values exist
	
	if (oldRowBuffer != ROW_BUFFER){
		ofxOscMessage spacerMessage;
		spacerMessage.setAddress("/pluginplay/calibration/buffers");
		spacerMessage.addFloatArg( ROW_BUFFER);
		calibrationSender.sendMessage(spacerMessage);
	}
	
	if (oldRowSpacing != ROW_SPACING){
		ofxOscMessage spaceMessage;
		spaceMessage.setAddress("/pluginplay/calibration/spacing");
		spaceMessage.addFloatArg( ROW_SPACING);
		calibrationSender.sendMessage(spaceMessage);
	}
	
	calcMeshNormals();
	
	//draw shadow map
	int w, h;		
	float halfFov, theTan, screenFov, aspect;
	
	w = ofGetWidth();
	h = ofGetHeight();		
	screenFov 		= 60.0f; //adjust this to alter the perspective
	float eyeX 		= (float)w / 2.0;
	float eyeY 		= (float)h / 2.0;
	ofxVec3f targetPos(eyeX, eyeX, 205);
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	float dist 		= eyeY / theTan;
	float nearDist		= 150; //dist / 50.0;	// near / far clip plane
	float farDist		= dist * 50.0;
	aspect 			= (float)w/(float)h;
	
	fbo.begin();{
		//if light source is moved, set up correct matrix to calc
		//if (bLightSourceChanged){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(screenFov, aspect, nearDist, farDist);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(lightPosition.x, lightPosition.y, lightPosition.z,
				  targetPos.x, targetPos.y, targetPos.z,
				  0.0, 1.0, 0.0);
		
		glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
		glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.
		//}
		
		//draw things
		glEnable(GL_DEPTH_TEST);
		ofPushMatrix();{
			//draw particles
			ofxLightsOff();
			particleManager->draw();
			ofxLightsOn();
			
			for(int i=0; i<stacks.size();i++){
				stacks[i].draw();
			}
			
			//grab matrices to pass to shadow shader
			grabMatrices(GL_TEXTURE7);
			bLightSourceChanged = false;			
		} ofPopMatrix();
		
		
		//copy depth buffer data to depthTexture.
		//was acting weird with the recent ofxFBO, so I switched to an older ofxFBO
		glBindTexture(GL_TEXTURE_2D, depthTexture);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
						 0, 0, fbo.getWidth(), fbo.getHeight(),0);
		
		ofDisableAlphaBlending();
		
	} fbo.end();
};

//--------------------------------------------------------------
void testApp::draw(){	
	if (ofGetElapsedTimeMillis() - fullScreenStarted < fullScreenWaitTime) return;
	
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
	
//draw geometry
	//set camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(screenFov, aspect, nearDist, farDist);	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(eyeX, eyeY, dist,
			  eyeX, eyeY, 0.0,
			  0.0, 1.0, 0.0);	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
	glTranslatef(0, -h, 0);       // shift origin up to upper-left corner.
	
	shadowShader.begin();{	
		shadowShader.setUniform("sampStep", 1.f/float(fbo.getWidth()), 1.f/float(fbo.getHeight()));	
		shadowShader.setUniform("lPos", lightPosition.x, lightPosition.y, lightPosition.z);	
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
			//get modelViewMatrix and invert it
			float modelView[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
			ofxMatrix4x4 invModelView(modelView);
			invModelView.makeInvertOf(invModelView);		
			//pass the inverse of modelViewMatrix to the shader
			glUniformMatrix4fv(glGetUniformLocationARB(shadowShader.shader, "invMat"),
							   1, GL_FALSE, invModelView.getPtr());
			
			if (bDragging){
				ofTranslate(ofGetWidth()/2.0f, ofGetHeight()/2.0f);
				ofRotate(mouseX, 0,1,0);
				ofTranslate(-ofGetWidth()/2.0f, -ofGetHeight()/2.0f);
			}
			
			//draw particles
			ofxLightsOff();
			particleManager->draw();
			ofxLightsOn();
			
			for(int i=0; i<stacks.size();i++){
				stacks[i].draw();
			}			
		} ofPopMatrix();	
			
		ofxLightsOff();		
	} shadowShader.end();			
		
	//draw conveyor
	//the texture uniforms need to be set for the shader
	shadowShader.begin();{
		shadowShader.setUniform("sampStep", 1.f/float(fbo.getWidth()), 1.f/float(fbo.getHeight()));	
		shadowShader.setUniform("lPos", lightPosition.x, lightPosition.y, lightPosition.z);	
		shadowShader.setUniform("ShadowMap", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthTexture);	
		shadowShader.setUniform("inTex", 0);
		glActiveTexture(GL_TEXTURE0);
		ceilingImage.getTextureReference().bind();
		
		glEnable(GL_DEPTH_TEST);
		if (bDragging){
			ofTranslate(ofGetWidth()/2.0f, ofGetHeight()/2.0f);
			ofRotate(mouseX, 0,1,0);
			ofTranslate(-ofGetWidth()/2.0f, -ofGetHeight()/2.0f);
		}
		drawConveyorMesh();
		
	} shadowShader.end();
	
	
	//draw curves
	
	if (bDrawCurves){
		glDisable(GL_DEPTH_TEST);
		glUseProgram(0);
		ofSetColor(0, 255, 0);
		for(int i=0; i<curves.size();i++)
			curves[i].drawWithGluNurbs();
	
		float increment = ROW_SPACING;
		
		for (int i=0; i<NUMBER_OF_ROWS; i++){
			ofLine(ROW_BUFFER + i*increment, 0, ROW_BUFFER + i*increment, ofGetHeight());
			ofCircle(ROW_BUFFER + i*increment, ofGetHeight()-20, 20);
		};
	}	
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
		}
	}
	
	loggerSender.sendMessage(newRowMessage);
	
	//Lars
	for (int i=0; i<completedRow->stacks.size(); i++){
		if(completedRow->stacks[i]->buildings.size() >0){
			stacks.push_back(*completedRow->stacks[i]);
			refVerts.push_back(getVertPointer(i, sweetSpot[i+1].uPos));
		}
	}		
};

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key =='f'){
		//fullScreenStarted = ofGetElapsedTimeMillis();
		//fullScreenWaitTime = 5000;
		ofToggleFullscreen();
	}
	if(key =='y')	moveConveyorY(1.);
	if(key =='u')	moveConveyorY(-1.);
	if (key == 'w'){
		wireFrame = !wireFrame;
		
		//draw wireFrame?
		if(wireFrame)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
		
	if(key == 's'){
		scaleConveyorY(float(mouseY)/float(1440.0f));
	}
	
	if ( key == '+' ) NUMBER_OF_ROWS++;
	else if (key == '-'){
		NUMBER_OF_ROWS--;
		if (NUMBER_OF_ROWS < 0 )NUMBER_OF_ROWS = 0;
	}
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
	lightPosition.x = x;
	bLightSourceChanged = true;
	cout<<lightPosition.x<<endl;
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
	cout<<"window resized"<<endl;
	if (!bInited){
		return;
	}
	
	if (particleManager != NULL){
		particleManager->windowResized();
	}	
	float xStep = w/float(numCurves);
	//float yStep = h/float(numCVs);
	
	for(int i=0; i<numCurves; i++){
		moveCurveX(i, i*xStep+xStep/2);
	}
	
	lightPosition.y = ofGetHeight()*.5;
	lightPosition.z = ofGetHeight()*1.5;
	
	//update screen vars
	screenWidth = ofGetWidth();
	screenHeight = ofGetHeight();		
	screenFov 		= 60.0f; //adjust this to alter the perspective
	eyeX 		= (float)screenWidth / 2.0;
	eyeY 		= (float)screenHeight / 2.0;
	targetPos.set(eyeX, eyeX, 205);
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	dist 		= eyeY / theTan;
	nearDist		= 150; //dist / 50.0;	// near / far clip plane
	farDist		= dist * 50.0;
	aspect 			= (float)screenWidth/(float)screenHeight;
}

//--------------------------------------------------------------
void testApp::grabMatrices(GLenum gl_textureTarget){
	
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] = { 
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};
	
	// Grab modelview and transformation matrices
	if (bLightSourceChanged || ofGetFrameNum() < 60){
		glGetDoublev(GL_MODELVIEW_MATRIX, shadowModelView); 
		glGetFloatv(GL_MODELVIEW_MATRIX, shadowModelViewFloat); 
		glGetDoublev(GL_PROJECTION_MATRIX, shadowProjection);
	}
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(gl_textureTarget);
	glLoadIdentity();
	glLoadMatrixd(bias);
	
	// concatating all matrices into one. 
	glMultMatrixd (shadowProjection);
	glMultMatrixd (shadowModelView);	
	
	// Go back to normal matrix mode 
	glMatrixMode(GL_MODELVIEW);
}

//------------------------------------------------------------------------------------------------
ofxVec3f testApp::normCalc( ofxVec3f a, ofxVec3f b, ofxVec3f c){
	ofxVec3f norm((a.z - b.z) * (c.y-b.y) - (a.y - b.y) * (c.z - b.z),
				  (a.x - b.x) * (c.z-b.z) - (a.z - b.z) * (c.x - b.x),
				  (a.y - b.y) * (c.x-b.x) - (a.x - b.x) * (c.y - b.y));
	
	return norm.normalize();
}

//------------------------------------------------------------------------------------------------
void testApp::calcMeshNormals(){
	
	for(int i=0;i<vertices.size();i++){
		vertices[i].norm.set(0,0,0);
	}
	for(int i=0; i<faceIndices.size(); i+=4){
		int normIndex = i/4;
		faceNorms[normIndex] = normCalc(vertices[faceIndices[i]].pos,
										vertices[faceIndices[i+1]].pos,
										vertices[faceIndices[i+2]].pos);
		faceNorms[normIndex] += normCalc(vertices[faceIndices[i]].pos,
										 vertices[faceIndices[i+2]].pos,
										 vertices[faceIndices[i+3]].pos);
		faceNorms[normIndex]/=2.;
		
		vertices[faceIndices[i]].norm	+= faceNorms[normIndex];
		vertices[faceIndices[i+1]].norm += faceNorms[normIndex];
		vertices[faceIndices[i+2]].norm += faceNorms[normIndex];
		vertices[faceIndices[i+3]].norm += faceNorms[normIndex];
	}	
	
	//make normals seemless
	for(int i=0; i<numCurves;i++){
		vertices[i*numSubdivisions + numSubdivisions-1].norm = vertices[i*numSubdivisions].norm;
	}
	
	for(int i=0;i<vertices.size();i++){
		vertices[i].norm.normalize();
	}
}

//------------------------------------------------------------------------------------------------
void testApp::drawConveyorMesh(){
	//draw faceted
		/*
	glBegin(GL_QUADS);
	for(int i=0;i<faceIndices.size();i++){
		
		int normIndex = floor(float(i)/4.f);
		glNormal3f(faceNorms[normIndex].x, faceNorms[normIndex].y, faceNorms[normIndex].z);
		//glNormal3f(vertices[faceIndices[i]].norm.x, vertices[faceIndices[i]].norm.y, vertices[faceIndices[i]].norm.z);
		glTexCoord2f(vertices[faceIndices[i]].u, vertices[faceIndices[i]].v);
		glVertex3f(vertices[faceIndices[i]].pos.x,
				   vertices[faceIndices[i]].pos.y,
				   vertices[faceIndices[i]].pos.z);
	}
	glEnd();
		 */
	
	//draw smoothed
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	
	glVertexPointer( 3, GL_FLOAT, sizeof(vertices[0]), &vertices[0].pos.x );
	glNormalPointer(GL_FLOAT, sizeof(vertices[0]), &vertices[0].norm.x );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(vertices[0]), &vertices[0].u );
	
	glDrawElements(GL_QUADS, faceIndices.size(), GL_UNSIGNED_INT, &faceIndices[0]);
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
}

void testApp::moveConveyorY(float dist){
	conveyorYoffset += dist;
	for(int i=0; i<curves.size();i++){
		for(int j=0; j<=curves[i].numCVs;j++){
			curves[i].moveCVrelative(j, ofxVec3f(0,dist,0));
		}
	}
	ceiling = conveyorYoffset + conveyorScale.y;
}
void testApp::scaleConveyor(int curveIndex, ofxVec3f scaleVal){
	//for(int i=0; i<curves.size();i++){
		for(int j=0; j<=curves[curveIndex].numCVs;j++){
			curves[curveIndex].moveCV(j, curves[curveIndex].getCVpos(j)*scaleVal);
		}
	//}
}
void testApp::moveCurveX(int crvIndex, float xPos){
	if (crvIndex > curves.size()) return;
	for(int j=0; j<=curves[crvIndex].numCVs;j++){
		ofxVec3f cvPos = curves[crvIndex].getCVpos(j);
		cvPos.x = xPos;
		curves[crvIndex].moveCV(j, cvPos);
	}
}

void testApp::setupConveyorMesh(){
	
	//define posiitons used t0 create curves
	refPoses.resize(numCVs);
	refPoses[0].set(0, 0.99, 0);
	refPoses[1].set(0, 1.03782, 0);
	refPoses[2].set(0, 1.15437, 0);//0.0157448);
	refPoses[3].set(0, 1.18961, 0);//0.130563);
	refPoses[4].set(0, 1.09218, 0.181413);
	refPoses[5].set(0, 0.986734, 0.191166);
	refPoses[6].set(0, 0.881457, 0.19796);
	refPoses[7].set(0, 0.776266, 0.193468);
	refPoses[8].set(0, 0.671478, 0.184758);
	refPoses[9].set(0, 0.567574, 0.171934);
	refPoses[10].set(0, 0.463153, 0.158185);
	refPoses[11].set(0, 0.360384, 0.144836);
	refPoses[12].set(0, 0.252854, 0.134736);
	refPoses[13].set(0, 0.155657, 0.121956);
	refPoses[14].set(0, 0.0421618, 0.0801633);
	refPoses[15].set(0, 0, 0.0206706);
	refPoses[16].set(0, 0.155657, 0.00257276);
	refPoses[17].set(0, 0.955657, 0);
	
	float xStep = ofGetWidth()/float(numCurves);
	float yStep = float (ofGetHeight())/float(numCVs);
	curves.reserve(numCurves);
	curves.resize(numCurves);
	conveyorYoffset = 135;
	
	float curX = ROW_BUFFER;
	conveyorScale.set(1.0, ofGetHeight()*.5, 1000.0);
		
	for(int i=0; i<numCurves; i++){
		for(int j=0; j<numCVs;j++){
			curves[i].addCV(curX,//i*xStep+xStep/2,
							refPoses[j].y * conveyorScale.y + conveyorYoffset,//sin(float(j)/float(numCVs))*300+ofGetHeight()*.5,
							refPoses[j].z * conveyorScale.z);//cos(float(j)/float(numCVs))*300);
			curves[i].setDegree(2);
			curves[i].setKnots();
			curves[i].setNumSpans(numSubdivisions);
			curX += ROW_SPACING;
		}		
	}
	conveyorScale.set(1.0, 1.0, 1.0);//freezes currnt scale at 1.0
	
	meshNodes.reserve(numSubdivisions*numCurves);
	meshNodes.resize(numSubdivisions*numCurves);	
	vertices.reserve(numSubdivisions*numCurves);	
	vertices.resize(numSubdivisions*numCurves);
	sweetSpot.resize(numCurves);	
	sweetSpotPos.resize(numCurves);	
	
	for(int i=0; i<numCurves;i++){
		for(int j=0; j<numSubdivisions;j++){
			float uVal = float(j)/float(numSubdivisions-1);
			vertices[i*numSubdivisions + j].u = uVal;
			vertices[i*numSubdivisions + j].v = float(i)/float(numCurves-1);
			
			meshNodes[i*numSubdivisions + j].setup(uVal,
												   &curves[i],
												   &vertices[i*numSubdivisions+j].pos);
		}
		
		sweetSpot[i].setup(.125, &curves[i], &sweetSpotPos[i]);
	}
	
	for(int i=0; i<numCurves-1;i++){
		for(int j=0; j<numSubdivisions-1;j++){
			faceIndices.push_back(i*numSubdivisions+j);
			faceIndices.push_back(i*numSubdivisions+j+1);
			faceIndices.push_back((i+1)*numSubdivisions+j+1);
			faceIndices.push_back((i+1)*numSubdivisions+j);
		}
	}
	faceNorms.resize(faceIndices.size()/4);
}

void testApp::scaleConveyorY(float scale){
	scale = fmax(.01, scale);
	
	float maxY = -1;
	
	for(int i=0; i<numCurves; i++){
		for(int j=0; j<numCVs;j++){
			ofxVec3f cvPos = curves[i].getCVpos(j);
			cvPos.y -= conveyorYoffset;
			cvPos.y /= conveyorScale.y;
			cvPos.y *= scale;
			cvPos.y += conveyorYoffset;
			
			curves[i].moveCV(j, cvPos );
			if (cvPos.y > maxY) maxY = cvPos.y;
		}		
	}
	
	//reset ceiling
	
	conveyorScale.y = scale;
	ceiling = conveyorYoffset + conveyorScale.y;
	particleManager->setCeiling(maxY);
	
	
	//reset texture coords of image
	
	float yImgScale = numCurves+numCurves*scale;	
	float xImgScale = numSubdivisions+numSubdivisions*scale;
	
	for(int i=0; i<numCurves;i++){
		for(int j=0; j<numSubdivisions;j++){
			float uVal = float(j)/float(xImgScale-1);
			vertices[i*numSubdivisions + j].u = uVal;
			vertices[i*numSubdivisions + j].v = float(i)/float(yImgScale-1);
		}
	}
	
	uIncrement = fmax(0.01, scale/500.0f);
	
	//advance all the mesh nodes
	for(int i=0;i<meshNodes.size();i++){
		meshNodes[i].uPos += uIncrement;
	}
	//delete any stacks that have a uPos > 1.0
	for(int i=refVerts.size()-1; i>=0;i--){
		if (meshNodes[refVerts[i]].uPos >= 1.0) {
			refVerts.erase(refVerts.begin()+i);
			stacks.erase(stacks.begin()+i);
		}
	}
	//wrap meshnodes that have grown bigger then 1.0 
	for(int i=0;i<meshNodes.size();i++){
		if(meshNodes[i].uPos >1.0)	meshNodes[i].uPos -= 1.0;
		if(meshNodes[i].uPos <0.0)	meshNodes[i].uPos += 1.0;
		meshNodes[i].update();		
	}
	
	//set stack position and rotation
	for(int i=refVerts.size()-1; i>=0;i--){
		stacks[i].setPosition(vertices[refVerts[i]].pos);
		//find rotation degree
		ofxVec3f upVec(0,1,0);
		stacks[i].angle = upVec.angle(vertices[refVerts[i]].norm) - 90;				
		stacks[i].rotAxis = ofxVec3f(1,0,0);//if we deform the surface more we need to 
		//set axis perpendicular to normal
	}
		
	//sweetSpot.uPos = float(mouseY)/float(ofGetHeight());
	for(int i=0;i<sweetSpot.size();i++){
		sweetSpot[i].update();
	}
	
	float oldRowBuffer = ROW_BUFFER;
	float oldRowSpacing = ROW_SPACING;	
	
	ROW_BUFFER = vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x;
	ROW_SPACING = vertices[getVertPointer(1, sweetSpot[2].uPos)].pos.x - vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x ;
	
	particleManager->setCeiling(vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.y);
}

int testApp::getVertPointer(int curveIndex, float uPos){
	float u = uPos - meshNodes[curveIndex*numSubdivisions].uPos;
	if(u > 1.0) u -= 1.0;
	if(u < 0.0) u += 1.0;
	return MIN(curveIndex*numSubdivisions + MIN(int(u*numSubdivisions), numSubdivisions-1), vertices.size()-1);
}

void testApp::exit(){
	delete particleManager;
	vertices.clear();
	faceIndices.clear();
	faceNorms.clear();
	curves.clear();
	meshNodes.clear();
	refPoses.clear();	
	sweetSpot.clear();
	sweetSpotPos.clear();
	
	fbo.clear();
};
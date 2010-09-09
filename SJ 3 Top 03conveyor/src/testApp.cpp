#include "testApp.h"

// settings vars

float conveyorScaleY;

/*********************************************************************************
	SETUP
*********************************************************************************/

void testApp::setup(){
	
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(120);
	ofHideCursor();
	ofDisableArbTex();
	
	//*** LIGHTS ************************************************
	
	ofxMaterialSpecular(120, 120, 120); //how much specular light will be reflect by the surface
	ofxMaterialShininess(30); //how concentrated the reflexion will be (between 0 and 128
	
	//*** SCREEN MATRIX ************************************************
	
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
	
	//*** SHADERS ************************************************
	
	depthShader.setup("shaders/depthShader");
	shadowShader.setup("shaders/shadow");	
	simpleTex.setup("shaders/facingRatio");	
	simpleTex.setUniform("inTex", 0);
	
	//*** FBO + DEPTH ************************************************
	
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
	
	//*** BASE VARS ************************************************
	
	numCurves = NUMBER_OF_ROWS;//number of stacks per row + 2(border)
	numCVs = 18;
	numSubdivisions = 25;
	uIncrement = .02; //this gets added to advance mesh verts and stack positions. added to uVal per frame
	
	lightPosition.x = projection.getView(0)->width;
	lightPosition.y = projection.getView(0)->height*.5;
	lightPosition.z = projection.getView(0)->height*1.5;
	bLightSourceChanged = true;
	
	fCounter = 0.0;
	wireFrame = false;
	bDrawCurves = false;
	bDragging = false;
	fullScreenStarted = ofGetElapsedTimeMillis();
	fullScreenWaitTime = 5000;
		
	//*** CALIBRATION ************************************************ 
	
	//setup view if not loaded via xml
	if (!projection.bSettingsLoaded){
		projection.addView(0,0,ofGetWidth(), ofGetHeight());
	}
	lightPosition.x = projection.getView(0)->width;
	lightPosition.y = projection.getView(0)->height*.5;
	lightPosition.z = projection.getView(0)->height*1.5;
	
	//setup gui
	drawMode = 0;
	setupGui();
	
	//load settings from xml
	loadSettings();
	
	//*** BUILD EMITTER ************************************************
	
	particleManager = new Emitter(ceiling);
	particleManager->setAdvanceCeilingBool(&advanceCeiling);
	
	//*** ROW CALIBRATION ************************************************ 
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
		
	//*** SETUP + SCALE CONVEYOR ************************************************ 
	
	setupConveyorMesh();
	scaleConveyorY(conveyorScaleY);
	bInited = true;
	bWindowResized = true;
	bSaveSettings = false;
	lastEmitted = 0;
}

/*********************************************************************************
	UPDATE
*********************************************************************************/

void testApp::update(){
	if ( ofGetElapsedTimeMillis() - lastEmitted > 200){
		lastEmitted = ofGetElapsedTimeMillis();
		//particleManager->emitRandom();
	}
	
	//yikes, gross hack
	//if (ofGetElapsedTimeMillis() - fullScreenStarted < fullScreenWaitTime) return;
	
	//*** GET EMITTER MESSAGES ************************************************ 
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
	
	//*** NEW VALUES FROM GUI ************************************************ 
	
	ofxLabGui * gui = projection.getGui();
	
	particleManager->update();
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
	
	//*** ADVANCE CONVEYOR ************************************************ 
	
	if(advanceCeiling){
		//advance all the mesh nodes
		for(int i=0;i<meshNodes.size();i++){
			meshNodes[i].uPos += uIncrement;
		}
		//delete any stacks that have a uPos > 1.0
		for(int i=refVerts.size()-1; i>=0;i--){
			if (meshNodes[refVerts[i]].uPos >= 1.0) {
				refVerts.erase(refVerts.begin()+i);
				if (stacks.size() > i) stacks.erase(stacks.begin()+i);
			}
		}
		//wrap meshnodes that have grown bigger then 1.0 
		for(int i=0;i<meshNodes.size();i++){
			if(meshNodes[i].uPos >1.0){
				meshNodes[i].uPos -= 1.0;
			}
			if(meshNodes[i].uPos <0.0){
				meshNodes[i].uPos += 1.0;
			}
			meshNodes[i].update();
		}
		
		
		//set stack position and rotation
		for(int i=refVerts.size()-1; i>=0;i--){
			if (stacks.size() > i){
				stacks[i].setPosition(vertices[refVerts[i]].pos);
			//find rotation degree
				ofxVec3f upVec(0,1,0);
				float angle = upVec.angle(vertices[refVerts[i]].norm) - 90;
				
				if (vertices[refVerts[i]].norm.z < 0){
					angle += 180;
					angle *= -1;
				}
				
				stacks[i].lastAngle = angle;
				stacks[i].angle = angle;
								
				stacks[i].rotAxis = ofxVec3f(1/*vertices[refVerts[i]].norm.y*/,0,0);
			}
		}
	}
	//sweetSpot.uPos = float(mouseY)/float(ofGetHeight());
	for(int i=0;i<sweetSpot.size();i++){
		sweetSpot[i].update();
	}
		
	//*** UPDATE ROW SPACING ************************************************ 
	
	float oldRowBuffer = ROW_BUFFER;
	float oldRowSpacing = ROW_SPACING;	
	
	//ROW_BUFFER = vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x;
	ROW_SPACING = vertices[getVertPointer(1, sweetSpot[2].uPos)].pos.x - vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x ;
	
	particleManager->setCeiling(vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.y);
	
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
	
	//*** LIGHT AND SHADOW SETUP ************************************************ 
	// RENDER HERE SO WE CAN RENDER OUR WHOLE SCENE TO AN FBO
	// WITHOUT EFFING UP THE MATRICES
	
	calcMeshNormals();
		
	if (bSaveSettings) saveSettings();
};

/*********************************************************************************
	DRAW
*********************************************************************************/

void testApp::draw(){	
	//if (ofGetElapsedTimeMillis() - fullScreenStarted < fullScreenWaitTime) return;
	
	//draw shadow map
	glPushMatrix();
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
		glTranslatef(0, -screenHeight, 0);       // shift origin up to upper-left corner.
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
	glPopMatrix();
	
	projection.pushView(0);
	
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
	//glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
	//glTranslatef(0, -screenHeight, 0);       // shift origin up to upper-left corner.
	
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
		for(int i=0; i<curves.size();i++){
			curves[i].drawWithGluNurbs();
		}
		
		float curX = ROW_BUFFER + ROW_SPACING;
		
		for (int i=0; i<NUMBER_OF_ROWS; i++){
			ofLine(curX, 0, curX, ofGetHeight());
			ofCircle(curX, ofGetHeight()-20, 20);
			curX += ROW_SPACING;
		};
	}	
	projection.popView();
	ofxLightsOff();
	ofSetColor(0xffffff);
	projection.draw();
}

/*********************************************************************************
	ROW COMPLETE EVENT
*********************************************************************************/

// this is where things get forwared to the database 
// + items get added to stacks

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
			completedRow->stacks[i]->angle = -270;
			stacks.push_back(*completedRow->stacks[i]);
			refVerts.push_back(getVertPointer(i+1, sweetSpot[i+1].uPos));
		}
	}		
};

/*********************************************************************************
	 KEY
*********************************************************************************/

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key =='f'){
		//fullScreenStarted = ofGetElapsedTimeMillis();
		//fullScreenWaitTime = 5000;
		ofToggleFullscreen();
	} else if(key =='y'){
		moveConveyorY(1.);
	} else if(key =='u'){
		moveConveyorY(-1.);
	} else if (key == 'w'){
		wireFrame = !wireFrame;
		
		//draw wireFrame?
		if(wireFrame)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	} /*else if(key == 's'){
		scaleConveyorY(float(mouseY)/float(projection.getView(0)->width));
	} */else if (key == 'm'){
		drawMode++;
		if (drawMode > NUM_DRAW_MODES-1){
			drawMode = 0;
			projection.setMode(0);
		}
		
		projection.setMode(drawMode);
		cout<<"set draw mode "<<drawMode<<endl;
		
	} else if (key == 'e'){
		particleManager->emitRandom();
	} else if (key == 's'){
		saveSettings();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

/*********************************************************************************
	MOUSE
*********************************************************************************/

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	bDragging = true;
	lightPosition.x = x;
	bLightSourceChanged = true;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	bDragging = false;
}

/*********************************************************************************
	WINDOW
*********************************************************************************/

void testApp::windowResized(int w, int h){
	if (!bInited){
		return;
	}
	
	if (particleManager != NULL){
		particleManager->windowResized();
	}	
	float xStep = (h + ROW_SPACING)/float(numCurves);
	//float yStep = h/float(numCVs);
	
	for(int i=0; i<numCurves; i++){
		moveCurveX(i, i*xStep+ROW_BUFFER);//+xStep/2);
	}
	
	//update screen vars
	screenWidth		= w;
	screenHeight	= h;		
	screenFov 		= 60.0f; //adjust this to alter the perspective
	eyeX			= (float)screenWidth / 2.0;
	eyeY			= (float)screenHeight / 2.0;
	targetPos.set(eyeX, eyeX, 205);
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	dist			= eyeY / theTan;
	nearDist		= 150; //dist / 50.0;	// near / far clip plane
	farDist			= dist * 50.0;
	aspect 			= (float)screenWidth/(float)screenHeight;
	bLightSourceChanged = true;
	
}


/*********************************************************************************
	CALIBRATION
*********************************************************************************/

//--------------------------------------------------------------
void testApp::setupGui(){
	ofxLabGui * gui = projection.getGui();
	//guiTypePanel * panel = projection.addDefaultPanel("particles");
	//projection.addDefaultGroup("settings", true);
	//projection.loadGuiSettings();
	
	//gui->setPanelIndex("particles", 0);
	gui->update();
	gui->setupOscReceiving(3001);
};

//--------------------------------------------------------------
void testApp::loadSettings(){
	string ceilingImagePath = "_ceiling_huge.jpg";	
	conveyorScaleY = 1.5f;
	conveyorY = 135;
	
	ofxXmlSettings settings;
	settings.loadFile("settings/settings.xml");
	settings.pushTag("settings");{
		
		//get port for receiver
		int rPort = 12001;
		rPort = settings.getValue("osc:receiver:port",rPort);
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
		
		ceilingImagePath = settings.getValue("conveyor:backgroundimage",ceilingImagePath);
		conveyorScaleY = settings.getValue("conveyor:conveyorScaleY",conveyorScaleY);
		ROW_BUFFER = settings.getValue("conveyor:rowBuffer",ROW_BUFFER);
		NUMBER_OF_ROWS = settings.getValue("conveyor:rowCount",NUMBER_OF_ROWS);
		uIncrement = settings.getValue("conveyor:uIncrement", uIncrement);
		conveyorY = settings.getValue("conveyor:conveyorY", conveyorY);
		
		lightPosition.x = settings.getValue("light:position:x",lightPosition.x);
		lightPosition.y = settings.getValue("light:position:y",lightPosition.y);
		lightPosition.z = settings.getValue("light:position:z",lightPosition.z);
		
	} settings.popTag();

	ceilingImage.loadImage(ceilingImagePath);
};

//--------------------------------------------------------------

void testApp::saveSettings(){
	ofxXmlSettings settings;
	settings.loadFile("settings/settings.xml");
	settings.setValue("settings:conveyor:conveyorScaleY",conveyorScaleY);
	settings.setValue("settings:conveyor:rowBuffer",ROW_BUFFER);
	settings.setValue("settings:conveyor:rowCount",NUMBER_OF_ROWS);
	settings.setValue("settings:conveyor:conveyorY", conveyorY);
	settings.setValue("settings:light:position:x",lightPosition.x);
	settings.setValue("settings:light:position:y",lightPosition.y);
	settings.setValue("settings:light:position:z",lightPosition.z);
	settings.setValue("settings:conveyor:uIncrement", uIncrement);
	
	settings.saveFile("settings/settings.xml");
};

/*********************************************************************************
	 LARS' 3D UTILS
*********************************************************************************/

//--------------------------------------------------------------
void testApp::grabMatrices(GLenum gl_textureTarget){
	
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] = { 
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};
	
	// Grab modelview and transformation matrices
	if (bLightSourceChanged || ofGetFrameNum() < 120){
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

/*********************************************************************************
	CONVEYOR RENDERING + UTILS
*********************************************************************************/

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
	glNormalPointer( GL_FLOAT, sizeof(vertices[0]), &vertices[0].norm.x );
	glTexCoordPointer( 2, GL_FLOAT, sizeof(vertices[0]), &vertices[0].u );
	
	glDrawElements(GL_QUADS, faceIndices.size(), GL_UNSIGNED_INT, &faceIndices[0]);
	
	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	
	bool bDebugNormals = false;
	if (bDebugNormals){		
		glBegin(GL_LINES);
		for (int i=0; i<vertices.size(); i++){
			glVertex3f(vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z);
			glVertex3f(vertices[i].pos.x+vertices[i].norm.x*100.0f, vertices[i].pos.y+vertices[i].norm.y*100.0f, vertices[i].pos.z+vertices[i].norm.z*100.0f);
		};
		glEnd();
	}		
}

//------------------------------------------------------------------------------------------------
void testApp::moveConveyorY(float dist){
	conveyorYoffset += dist;
	for(int i=0; i<curves.size();i++){
		for(int j=0; j<=curves[i].numCVs;j++){
			curves[i].moveCVrelative(j, ofxVec3f(0,dist,0));
		}
	}
	ceiling = conveyorYoffset + conveyorScale.y;
}


//------------------------------------------------------------------------------------------------
void testApp::scaleConveyor(int curveIndex, ofxVec3f scaleVal){
	//for(int i=0; i<curves.size();i++){
		for(int j=0; j<=curves[curveIndex].numCVs;j++){
			curves[curveIndex].moveCV(j, curves[curveIndex].getCVpos(j)*scaleVal);
		}
	//}
}

//------------------------------------------------------------------------------------------------
void testApp::moveCurveX(int crvIndex, float xPos){
	if (crvIndex > curves.size()) return;
	for(int j=0; j<=curves[crvIndex].numCVs;j++){
		ofxVec3f cvPos = curves[crvIndex].getCVpos(j);
		cvPos.x = xPos;
		curves[crvIndex].moveCV(j, cvPos);
	}
}

//------------------------------------------------------------------------------------------------
void testApp::setupConveyorMesh(){
	
	//define posiitons used t0 create curves
	refPoses.resize(numCVs);
	refPoses[0].set(0, 0.99, 0);
	refPoses[1].set(0, 1.03782, 0);
	refPoses[2].set(0, 1.15437, 0);//0.0157448);
	refPoses[3].set(0, 1.09218, 0.130563);
	//refPoses[3].set(0, 1.18961, 0.130563);
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
	conveyorYoffset = conveyorY;
	
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

//------------------------------------------------------------------------------------------------
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
		
	//advance all the mesh nodes
	for(int i=0;i<meshNodes.size();i++){
		meshNodes[i].uPos += uIncrement;
	}
	//delete any stacks that have a uPos > 1.0
	for(int i=refVerts.size()-1; i>=0;i--){
		if (meshNodes[refVerts[i]].uPos >= 1.0) {
			refVerts.erase(refVerts.begin()+i);
			if (stacks.size() > i) stacks.erase(stacks.begin()+i);
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
		if (stacks.size() > i){
			stacks[i].setPosition(vertices[refVerts[i]].pos);
			//find rotation degree
			ofxVec3f upVec(0,1,0);
			stacks[i].angle = upVec.angle(vertices[refVerts[i]].norm) - 90;				
			stacks[i].rotAxis = ofxVec3f(1/*vertices[refVerts[i]].norm.y*/,0,0);//if we deform the surface more we need to 
			//set axis perpendicular to normal
		}
	}
		
	//sweetSpot.uPos = float(mouseY)/float(ofGetHeight());
	for(int i=0;i<sweetSpot.size();i++){
		sweetSpot[i].update();
	}
	
	float oldRowBuffer = ROW_BUFFER;
	float oldRowSpacing = ROW_SPACING;	
	
	//ROW_BUFFER = vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x;
	ROW_SPACING = vertices[getVertPointer(1, sweetSpot[2].uPos)].pos.x - vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.x ;
	
	particleManager->setCeiling(vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.y);
	
	//send calibration?
	
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
	
	particleManager->setCeiling(vertices[getVertPointer(0, sweetSpot[1].uPos)].pos.y);
	bSaveSettings = true;
}

//------------------------------------------------------------------------------------------------
int testApp::getVertPointer(int curveIndex, float uPos){
	float u = uPos - meshNodes[curveIndex*numSubdivisions].uPos;
	if(u > 1.0) u -= 1.0;
	if(u < 0.0) u += 1.0;
	return MIN(curveIndex*numSubdivisions + MIN(int(u*numSubdivisions), numSubdivisions-1), vertices.size()-1);
}

//------------------------------------------------------------------------------------------------
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
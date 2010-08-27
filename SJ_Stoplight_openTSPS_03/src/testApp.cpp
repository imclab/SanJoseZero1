#include "testApp.h"


class TSPSPersonAttributes {
public:
	TSPSPersonAttributes(){
		height = 0;
		hasBeard = false;
	}

	float height;
	bool hasBeard;
};

/*--------------------------------------------------------------
 NOTE: IF YOU'RE HAVING TROUBLE COMPILING, TRY SETTING
 dataPathRoot to "../../../data" in the OF static library:

 #if defined TARGET_OSX
	static string dataPathRoot = "../../../data/";
 #else
	static string dataPathRoot = "data/";
 #endif
-------------------------------------------------------------*/


//--------------------------------------------------------------
void testApp::setup(){
	theX = 0;
	theY = 0;
	r = 255;
	g = 0;
	b = 0;
	redLight.red = 255;
	redLight.green = 0;
	redLight.blue = 0;
	yellowLight.red = 255;
	yellowLight.green = 255;
	yellowLight.blue = 0;
	greenLight.red = 0;
	greenLight.green = 255;
	greenLight.blue = 0;
	lightIndex = 0;
	colorSet = -1;
	colorSetCtr = 0;
	
	
	
	
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	ofBackground(223, 212, 190);
	
	camWidth = 640;
	camHeight = 480;

	#ifdef _USE_LIVE_VIDEO
		vidGrabber.setVerbose(true);
//		vidGrabber.videoSettings();
        vidGrabber.initGrabber(camWidth,camHeight);
	#else
        vidPlayer.loadMovie("testmovie/twoPeopleStand.mov");
        vidPlayer.play();
		camWidth = vidPlayer.width;
		camHeight = vidPlayer.height;
	#endif

	colorImg.allocate(camWidth, camHeight);
	warpedImage.allocate(camWidth, camHeight);
	
	peopleTracker.setup(camWidth, camHeight);
	peopleTracker.loadFont("fonts/times.ttf", 10);
	peopleTracker.setListener( this );
	peopleTracker.enableOpticalFlow(true);
	
	// Set up OSC sender client
	stoplight.sender = peopleTracker.getOSCsender();

	opticalFlowDetectionThreshold = peopleTracker.getOpticalFlowThreshold();
//	particleEmitThresholdSeconds = peopleTracker.getParticleEmitThresholdSeconds();
	
	/*
	//THIS IS HOW YOU CAN ADD CUSTOM PARAMETERS TO THE GUI
	peopleTracker.addSlider("custom INTEGER", &itestValue, 0, ofGetWidth());
	peopleTracker.addSlider("custom FLOAT", &ftestValue, 0, ofGetHeight());
	peopleTracker.addToggle("custom BOOL", &btestValue);
	*/
	peopleTracker.setActiveDimensions( ofGetWidth(), ofGetHeight()-68 );

	//load GUI / interface images

	personEnteredImage.loadImage("graphic/triggers/PersonEntered_Active.png");
	personUpdatedImage.loadImage("graphic/triggers/PersonUpdated_Active.png");
	personLeftImage.loadImage("graphic/triggers/PersonLeft_Active.png");
	statusBar.loadImage("graphic/bottomBar.png");
	background.loadImage("graphic/background.png");

	timesBoldItalic.loadFont("fonts/timesbi.ttf", 16);

	drawStatus[0] = 0;
	drawStatus[1] = 0;
	drawStatus[2] = 0;
	drawMode = 0;
	
	
	// ZACK: Initialize last emit times for each quad to the current time
//	for (int i = 0; i < stoplight.numRects; i++) {
//		time(&lastEmitTimes[i]);
//	}
}

//--------------------------------------------------------------
void testApp::update(){
	
    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.grabFrame();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.idleMovie();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels(), camWidth,camHeight);
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels(), camWidth,camHeight);
        #endif
		peopleTracker.update(colorImg);

		//iterate through the people
		for(int i = 0; i < peopleTracker.totalPeople(); i++){
			ofxTSPSPerson* p = peopleTracker.personAtIndex(i);
			//now make sweet interactivity with these people!
		}
	}
	
	
	
	// OPTICAL FLOW
	// ZACK: Get optical flow for the current light (quad) we are sensing.
	bool bOpticalFlowTriggered = false;
	Quad* q = stoplight.getQuad(lightIndex);
	float x1,y1,w1,h1;
	// Get the quad points / region
	x1 = (*q).p1.x / 2.0;
	y1 = (*q).p1.y / 2.0;
	w1 = ((*q).p2.x - (*q).p1.x) / 2.0;
	h1 = ((*q).p4.y - (*q).p1.y) / 2.0;
	// Get the optical flow for the quad
	ofPoint myPoint = peopleTracker.getOpticalFlowInRegion(x1,y1,w1,h1);
	// Test that enough movement has occured to trigger a sendSignal OSC event
	if (fabs(myPoint.x) + fabs(myPoint.y) > *opticalFlowDetectionThreshold) {
		bOpticalFlowTriggered = true;
	}

	
	
	// COLOR SENSING
	// ZACK: Search the pixels of the given light (quad) to see if we have a matching light
	bool bMatchedColor = false;
	Quad* lightQ = stoplight.getQuad(lightIndex);
	int x,y,w,h;
	// Get the quad points / region
	x = (int)((*lightQ).p1.x);// / 2.0);
	y = (int)((*lightQ).p1.y);// / 2.0);
	w = ((*lightQ).p2.x - (*lightQ).p1.x);// / 2.0;
	h = ((*lightQ).p4.y - (*lightQ).p1.y);// / 2.0;
	// Get the image and region of interest (ROI) whose pixels will be searched
	warpedImage = peopleTracker.getAdjustedColorImage();
	warpedImage.resetROI();
	warpedImage.setROI(x,y,w,h);
	// Get and iterate through the pixels for the quad
	unsigned char* px = warpedImage.getRoiPixels();
	ofxColor curPixel;
	float colorDist;
	for (int j = 0; j < (w * h); j++) {
		curPixel.red = (int)px[j * 3];
		curPixel.green = (int)px[j * 3 + 1];
		curPixel.blue = (int)px[j * 3 + 2];
		
		if (lightIndex == 0) {
			colorDist = redLight.distance(curPixel,OF_COLOR_RGB);
		} else if (lightIndex == 1) {
			colorDist = yellowLight.distance(curPixel,OF_COLOR_RGB);
		} else {
			colorDist = greenLight.distance(curPixel,OF_COLOR_RGB);
		}
		
		// See if the current pixel matches the set color for this quad
		if (colorDist < 0.1) {
			bMatchedColor = true;
			break;
		}
	}
	warpedImage.resetROI();
	

	
	// OSC SIGNAL
	// ZACK BOKA: Send OSC message based on what kind of sensing we are doing and change the lightIndex if an OSC even has been triggered above
	if (peopleTracker.bColorSensingEnabled && peopleTracker.bOpticalFlowEnabled) {
		if (bMatchedColor && bOpticalFlowTriggered) {
			if (DEBUG) {
				cout << "BOTH\n";
				cout << "light: " << (*lightQ).name << ", colorDist: " << colorDist << ", " << curPixel.red << ":" << curPixel.green << ":" << curPixel.blue << endl;
			}
			sendOscMessage(lightIndex);
				if (lightIndex == 0)
				lightIndex = 2;
			else
				lightIndex--;
		}
	} else if (peopleTracker.bColorSensingEnabled) {
		if (bMatchedColor) {
			if (DEBUG) {
				cout << "COLOR SENSING\n";
				cout << "light: " << (*lightQ).name << ", colorDist: " << colorDist << ", " << curPixel.red << ":" << curPixel.green << ":" << curPixel.blue << endl;
			}
			sendOscMessage(lightIndex);
				if (lightIndex == 0)
				lightIndex = 2;
			else
				lightIndex--;
		}
	} else if (peopleTracker.bOpticalFlowEnabled) {
		if (bOpticalFlowTriggered) {
			if (DEBUG) {
				cout << "OPTICAL FLOW\n";
				cout << "light: " << (*lightQ).name << ", colorDist: " << colorDist << ", " << curPixel.red << ":" << curPixel.green << ":" << curPixel.blue << endl;
			}
			sendOscMessage(lightIndex);
			if (lightIndex == 0)
				lightIndex = 2;
			else
				lightIndex--;
		}
	}
}

// ZACK: sends an OSC message for the given quad
void testApp::sendOscMessage(int quadIndex) {
	time_t sendTime;
	time(&sendTime);
	
	lastEmitTimes[quadIndex] = sendTime;
	stoplight.send(quadIndex);

	// Test that enough time has elapsed since the last sent message for the given quad
//	if (difftime(sendTime,lastEmitTimes[quadIndex]) >= *particleEmitThresholdSeconds) {
//		lastEmitTimes[quadIndex] = sendTime;
//		stoplight.send(quadIndex);
//		if (DEBUG) cout << "emitted: " << stoplight.names[quadIndex] << endl;
//	} else {
//		if (DEBUG) cout << "too soon\n";
//	}
}


//delegate methods for people entering and exiting
void testApp::personEntered( ofxTSPSPerson* newPerson, ofxTSPSScene* scene )
{
	
	newPerson->customAttributes = new TSPSPersonAttributes();

	//do something with them
	ofLog(OF_LOG_VERBOSE, "person %d of size %f entered!\n", newPerson->pid, newPerson->area);
	drawStatus[0] = 10;

	Quad q = stoplight.testSquare(newPerson->boundingRect.x+newPerson->boundingRect.width/2, newPerson->boundingRect.y+newPerson->boundingRect.height/2);
	if (q.active){
//		cout << "In an active square!   " << q.index << endl;
//		stoplight.send(q.index, "personEntered");
	}
	
}

void testApp::personMoved( ofxTSPSPerson* activePerson, ofxTSPSScene* scene )
{
	
	//do something with the moving person
	ofLog(OF_LOG_VERBOSE, "person %d of moved to (%f,%f)!\n", activePerson->pid, activePerson->boundingRect.x, activePerson->boundingRect.y);
	drawStatus[1] = 10;
	
	Quad q = stoplight.testSquare(activePerson->boundingRect.x+activePerson->boundingRect.width/2, activePerson->boundingRect.y+activePerson->boundingRect.height/2);
	if (q.active){
//		cout << "In an active sqaure (Moved)   " << q.index << endl;
//		stoplight.send(q.index, "personMoved");
	}
	
}

void testApp::personWillLeave( ofxTSPSPerson* leavingPerson, ofxTSPSScene* scene )
{
	//do something to clean up
	ofLog(OF_LOG_VERBOSE, "person %d left after being %d frames in the system\n", leavingPerson->pid, leavingPerson->age);
	drawStatus[2] = 10;
}

void testApp::personUpdated( ofxTSPSPerson* updatedPerson, ofxTSPSScene* scene )
{
	TSPSPersonAttributes* attrbs = (TSPSPersonAttributes*)updatedPerson->customAttributes;
	attrbs->hasBeard = true;

	ofLog(OF_LOG_VERBOSE, "updated %d person\n", updatedPerson->pid);
	drawStatus[1] = 10;
	
	Quad q = stoplight.testSquare(updatedPerson->boundingRect.x+updatedPerson->boundingRect.width/2, updatedPerson->boundingRect.y+updatedPerson->boundingRect.height/2);
	if (q.active){
//		stoplight.send(q.index, "personUpdated");
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofEnableAlphaBlending();
	ofSetColor(0xffffff);
	ofPushStyle();
	background.draw(0,0);
	
	peopleTracker.draw();

	if(btestValue){
		ofCircle(itestValue, ftestValue, 30);
	}
	ofPopStyle();

	//draw status bar stuff

	statusBar.draw(0,700);//ofGetHeight()-statusBar.height);
	if (drawStatus[0] > 0){
		drawStatus[0]--;
		personEnteredImage.draw(397,728);
	}
	if (drawStatus[1] > 0){
		drawStatus[1]--;
		personUpdatedImage.draw(533,728);
	}
	if (drawStatus[2] > 0){
		drawStatus[2]--;
		personLeftImage.draw(666,728);
	}

	ofSetColor(0, 169, 157);
	char numPeople[1024];
	sprintf(numPeople, "%i", peopleTracker.totalPeople());
	timesBoldItalic.drawString(numPeople,350,740);

	if (drawMode == DRAW_MODE_STOPLIGHT){
		stoplight.draw();
	}
	
	//TESTING
	ofFill();
	ofSetColor(r,g,b);
	ofRect(theX,theY,100,100);
	
	ofSetColor(100,255,255);
	if (colorSetCtr) {
		if (colorSet == 0) {
			string rString = "RED LIGHT: setting sensing color";
			ofDrawBitmapString(rString, 362, 24);
		} else if (colorSet == 1) {
			string yString = "YELLOW LIGHT: setting sensing color";
			ofDrawBitmapString(yString,362,24);
		} else if (colorSet == 2) {
			string gString = "GREEN LIGHT: setting sensing color";
			ofDrawBitmapString(gString,362,24);
		}
		
		colorSetCtr--;
	}

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	switch (key){
		case ' ':{
			peopleTracker.relearnBackground();
		}break;
		case 'f':{
			ofToggleFullscreen();
		}break;
		case 'm':{
			drawMode++;
			if (drawMode > 1) drawMode = 0;
		}break;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if (drawMode == DRAW_MODE_STOPLIGHT){
		stoplight.mouseDragged(x, y);
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	// Did we click inside a quad and thus need to set the color for that quad?
	Quad testq = stoplight.testSquare(x - 360,y - 22);
	bool bSetColor = false;
	if (DEBUG) cout << "Mouse pressed coordinates: " << (x - 364) << ":" << (y - 22) << endl;
	if (testq.active) {
		if (DEBUG) cout << "CLICKED INSIDE QUAD: " << testq.name << endl;
		bSetColor = true;
	}
	
	// Set the coordinates for the 'color square'
	theX = x + 40;
	theY = y + 40;
	

	// Get the color of the pixel that was clicked on
	warpedImage.resetROI();
	warpedImage.setROI(x - 364,y - 22, 1, 1);   //(x - 352, y - 14, 1,1);
	unsigned char* px = warpedImage.getRoiPixels();
	warpedImage.resetROI();
	// Set the 'color square' color based on the pixel color
	r = (int)px[0];
	g = (int)px[1];
	b = (int)px[2];
	// Set the color for a quad if we clicked inside that quad
	if (bSetColor && peopleTracker.inAdjustedView()) {
		if (testq.name == "red") {
			redLight.red = r;
			redLight.green = g;
			redLight.blue = b;
			colorSet = 0;
			if (DEBUG) cout << "set color: " << r << ":" << g << ":" << b << endl;
		} else if (testq.name == "yellow") {
			yellowLight.red = r;
			yellowLight.green = g;
			yellowLight.blue = b;
			colorSet = 1;
			if (DEBUG) cout << "set color: " << r << ":" << g << ":" << b << endl;
		} else {
			greenLight.red = r;
			greenLight.green = g;
			greenLight.blue = b;
			colorSet = 2;
			if (DEBUG) cout << "set color: " << r << ":" << g << ":" << b << endl;
		}
		
		colorSetCtr = 30;
	}
	

	if (drawMode == DRAW_MODE_STOPLIGHT){
		stoplight.mousePressed(x, y);
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if (drawMode == DRAW_MODE_STOPLIGHT){
		stoplight.mouseReleased();
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}




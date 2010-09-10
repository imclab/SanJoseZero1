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
	
	peopleTracker.setup(camWidth, camHeight);
	peopleTracker.loadFont("fonts/times.ttf", 10);
	peopleTracker.setListener( this );
	peopleTracker.enableOpticalFlow(true);
	
	// Set up OSC sender client
	blocks.sender = peopleTracker.getOSCsender();
	
	// Initialize Optical Flow threshold
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
	// ZACK: get Optical Flow for each quad (blocks square)
	float x,y,w,h;
	Quad* q;
	for (int i = 0; i < blocks.numRects; i++) {
		q = blocks.getQuad(i);
		x = (*q).p1.x / 2.0;
		y = (*q).p1.y / 2.0;
		w = ((*q).p2.x - (*q).p1.x) / 2.0;
		h = ((*q).p4.y - (*q).p1.y) / 2.0;

		// Get the optical flow for the quad
		ofPoint myPoint = peopleTracker.getOpticalFlowInRegion(x,y,w,h);
		
		// Test that enough movement has occured to trigger a sendSignalOSC event
		if (fabs(myPoint.x) + fabs(myPoint.y) > *opticalFlowDetectionThreshold) {
			if (DEBUG) cout << "GOT IT! Quad: " << (*q).name << ",  " << myPoint.x << "," << myPoint.y << endl;
			sendOscMessage(i);
		}
	}
}

// ZACK: sends an OSC message for the given quad
void testApp::sendOscMessage(int quadIndex) {
	blocks.send(quadIndex);
}


//delegate methods for people entering and exiting
void testApp::personEntered( ofxTSPSPerson* newPerson, ofxTSPSScene* scene )
{
	
	newPerson->customAttributes = new TSPSPersonAttributes();

	//do something with them
	ofLog(OF_LOG_VERBOSE, "person %d of size %f entered!\n", newPerson->pid, newPerson->area);
	drawStatus[0] = 10;

	Quad q = blocks.testSquare(newPerson->boundingRect.x+newPerson->boundingRect.width/2, newPerson->boundingRect.y+newPerson->boundingRect.height/2);
	if (q.active){
//		cout << "In an active square!   " << q.index << endl;
//		blocks.send(q.index, "personEntered");
	}
	
}

void testApp::personMoved( ofxTSPSPerson* activePerson, ofxTSPSScene* scene )
{
	
	//do something with the moving person
	ofLog(OF_LOG_VERBOSE, "person %d of moved to (%f,%f)!\n", activePerson->pid, activePerson->boundingRect.x, activePerson->boundingRect.y);
	drawStatus[1] = 10;
	
	Quad q = blocks.testSquare(activePerson->boundingRect.x+activePerson->boundingRect.width/2, activePerson->boundingRect.y+activePerson->boundingRect.height/2);
	if (q.active){
//		cout << "In an active sqaure (Moved)   " << q.index << endl;
//		blocks.send(q.index, "personMoved");
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
	
	Quad q = blocks.testSquare(updatedPerson->boundingRect.x+updatedPerson->boundingRect.width/2, updatedPerson->boundingRect.y+updatedPerson->boundingRect.height/2);
	if (q.active){
//		blocks.send(q.index, "personUpdated");
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

	if (drawMode == DRAW_MODE_CDM_BLOCKS){
		blocks.draw();
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
	if (drawMode == DRAW_MODE_CDM_BLOCKS){
		blocks.mouseDragged(x, y);
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if (drawMode == DRAW_MODE_CDM_BLOCKS){
		blocks.mousePressed(x, y);
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if (drawMode == DRAW_MODE_CDM_BLOCKS){
		blocks.mouseReleased();
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
}




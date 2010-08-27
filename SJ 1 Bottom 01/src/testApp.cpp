#include <iostream>               // for std::cout
#include <vector>
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
		
	//load messagestrings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");
	
	numReceivers = settings.getNumTags("input");
		
	int emitDelay = 500;
	
	settings.pushTag("misc");{
		emitDelay = settings.getValue("emitdelay",emitDelay);
	} settings.popTag();
	
	// Loops through directories '0' through '9'.
	for (int i=0; i<numReceivers; i++) {
		
		Emitter * e = new Emitter();
		e->setLoc(ofGetWidth()/numReceivers * i,0);
		
		if (i <numReceivers){			
			settings.pushTag("input", i);{
				e->setName(settings.getValue("name", "") );
				for (int j=0; j<settings.getNumTags("message"); j++){
					settings.pushTag("message", j);
						e->addMessageString(settings.getValue("messageString", ""));
						e->loadImage(ofToDataPath(settings.getValue("image", "")));
						cout << "loading "<<settings.getValue("image", "")<<":"<<settings.getValue("messageString", "")<<endl;
					settings.popTag();
				}
				e->setSendSound( settings.getValue("sendsound", false) );
				e->setEmitDelay(emitDelay);
			} settings.popTag();
		};
		//add listener for particles leaving screen
		ofAddListener(e->particleLeft, this, &testApp::elementLeftScreen);
		
		emitters.push_back(e);
	}
	
	//get host + port for sender
	
	string host = "localhost";
	int port = 12000;
	int receiverPort = 12345;
	
	string soundHost = "localhost";
	int soundPort = 12002;
	
	settings.pushTag("osc");{
				
		settings.pushTag("receiver");{
			receiverPort = settings.getValue("port",12345);
		} settings.popTag();
		
		settings.pushTag("sender");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12000);
		} settings.popTag();
		
		settings.pushTag("soundsender");{
			soundHost = settings.getValue("host", soundHost);
			soundPort = settings.getValue("port",soundPort);
		} settings.popTag();
	} settings.popTag();
	
	settings.popTag();

	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofLog(OF_LOG_VERBOSE, "setting up receiver at "+ofToString(receiverPort));
	ofLog(OF_LOG_VERBOSE, "setting up sender at "+host+","+ofToString(port));
	ofLog(OF_LOG_VERBOSE, "setting up sound sender at "+soundHost+","+ofToString(soundPort));
	ofLog(OF_LOG_VERBOSE, "there are "+ofToString((int)emitters.size()));;
	
	//setup OSC
	receiver.setup( receiverPort );
	sender.setup(host, port);
	soundSender.setup(soundHost, soundPort);
		
	ofHideCursor();
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
		
		cout<<"got "<<m.getAddress()<<endl;
		for (int i=0; i<emitters.size(); i++){
			if (emitters[i]->checkMessageString(m.getAddress())){
				emitters[i]->emit(emitters[i]->lastFoundString);
				cout<<"send sound? "<<emitters[i]->sendSound()<<endl;
				if (emitters[i]->sendSound()){
					ofxOscMessage m;
					m.setAddress("/pluginplay/sound");
					m.addStringArg(emitters[i]->getName());
					soundSender.sendMessage(m);
				}
				bFound = true;
				break;
			}
		}
		
		// unrecognized message: display on screen
		if (!bFound)
		{
			ofLog(OF_LOG_ERROR, "caught unrecognized message "+m.getAddress());
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
	
	//emitterz
	// draw a quad that fills the whole screen
	glBegin(GL_QUADS);{
		glColor3f( 0.0f, 0.3f, 0.3f );
		glVertex3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( ofGetWidth(), 0.0f, 0.0f );
		glColor3f( 0.0f, 0.0f, 0.0f );
		glVertex3f( ofGetWidth(), ofGetHeight(), 0.0f );
		glVertex3f( 0.0f, ofGetHeight(), 0.0f );
	} glEnd();
	
	ofSetColor(0xffffff);
	ofEnableAlphaBlending();
	//draw particles
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->draw();
	}
	ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key =='f') ofToggleFullscreen();
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
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->setLoc(ofGetWidth()/numReceivers * i,0);
	}
}

//--------------------------------------------------------------
void testApp::elementLeftScreen( ParticleEventArgs & args ){
	ofxOscMessage m;
	m.setAddress(args.address);
	//normalize x coord
	m.addFloatArg( (float) args.loc.x );
	sender.sendMessage(m);	
};


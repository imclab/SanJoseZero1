#include <iostream>               // for std::cout
#include <vector>
#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	ofBackground( 106,186,205);
	ofSetFrameRate(60);
		
	//load font
	font.loadFont("fonts/rockwell.ttf", 12);
	
	//load messagestrings from xml
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");
	
	int numReceivers = settings.getNumTags("input");
	
	int emitDelay = 500;
	
	settings.pushTag("misc");{
		emitDelay = settings.getValue("emitdelay",emitDelay);
	} settings.popTag();
	
	boxX = 50;
	boxY = 10;
	
	for (int i=0; i<numReceivers; i++) {
		Emitter * e = new Emitter();	
		e->x = boxX;
		e->y = boxY;
		e->setFont(&font);
		
		if (i <numReceivers){			
			settings.pushTag("input", i);{
				e->setName(settings.getValue("name", "") );
				for (int j=0; j<settings.getNumTags("message"); j++){
					settings.pushTag("message", j);
						e->addMessageString(settings.getValue("messageString", ""));
					settings.popTag();
				}
				e->setSendSound( settings.getValue("sendsound", false) );
				if (!e->sendSound()) e->color.b += 20;
				e->setEmitDelay(emitDelay);
			} settings.popTag();
		};
		
		boxX += e->width+10;
		if (boxX + e->width > ofGetWidth()){
			boxX = 50;
			boxY += e->height + 10;
		};
		
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
	
	for (int i=0; i<NUM_STRINGS; i++){
		errorString[i] = "";
		debugString[i] = "";
	};
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
				
				//has it been long enough that it's OK to emit?
				
				bool bCanEmit = emitters[i]->emit(emitters[i]->lastFoundString);
				if (bCanEmit){
						
					//send "emit!" message to bottom app with its address + data
					
					ofxOscMessage forwardedMessage;
					forwardedMessage.setAddress(m.getAddress());
					
					//is there data?
					bool bHasData = false;
					if ( m.getNumArgs() > 0){
						bHasData = true;
						forwardedMessage.addStringArg( m.getArgAsString(0) );
					}
										
					sender.sendMessage(forwardedMessage);					
					
					bool bSendSound = emitters[i]->sendSound();
					
					if (bSendSound){
						ofxOscMessage m;
						m.setAddress("/pluginplay/sound");
						m.addStringArg(emitters[i]->getName());
						soundSender.sendMessage(m);
					}
					
					for (int i=NUM_STRINGS-1; i>=1; i--){
						debugString[i] = debugString[i-1];
					};
					
					if (bSendSound) debugString[0] = "Sent message and sound: "+m.getAddress()+" at frame "+ofToString(ofGetFrameNum())+"\n";
					else debugString[0] = "Sent message: "+m.getAddress()+" at "+ofToString(ofGetFrameNum())+"\n";
						
					if (bHasData)
						debugString[0] += m.getArgAsString(0)+"\n";
					
					cout<< debugString[0]<<endl;
				}
				bFound = true;
				break;
			}
		}
		
		// unrecognized message: display on screen
		if (!bFound)
		{
			ofLog(OF_LOG_ERROR, "caught unrecognized message "+m.getAddress());
			for (int i=NUM_STRINGS-1; i>=1; i--){
				errorString[i] = errorString[i-1];
			};
			errorString[0] = "ERROR: Unrecongnized message: "+m.getAddress()+" at frame "+ofToString(ofGetFrameNum())+"\n";
		}
		
	}
	
	ofSetWindowTitle("fps: "+ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){	
	ofSetColor(0xffffff);
	font.drawString("OSC Router\nFramerate: "+ofToString(ofGetFrameRate()), boxX, 14+boxY);
	/* for (int i=0; i<NUM_STRINGS; i++){
		ofSetColor(0xff0000);
		ofDrawBitmapString(errorString[i], 20, 40+20*i);
		ofSetColor(0x00ff00);
		ofDrawBitmapString(debugString[i], 20, 250+20*i);
	} */
	
	//draw text labels
    ofPushMatrix();{
		ofTranslate(10, 232+10);
		ofRotateZ(-90);
		ofSetColor(100,100,100);
		ofRect(0,0,232, 30);
		ofSetColor(200,200,200);
		font.drawString("SENSOR INPUT", 8, 14 + 4);
    } ofPopMatrix();
    
	ofPushMatrix();{
		ofTranslate(10, 232*2+20);
		ofRotateZ((-90));
		ofSetColor(100,100,100);
		ofRect(0,0,232, 30);
		ofSetColor(200,200,200);
		font.drawString("DIGITAL INPUT", 8, 14 + 4);
    } ofPopMatrix();
	
	for (int i=0; i<emitters.size(); i++){
		emitters[i]->draw();
	}	
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
}


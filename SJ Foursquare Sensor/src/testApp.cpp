#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
		
	lastTimeChecked = 0;
	bChecked = false;
	
	venueDetails.setAuth("brenfer@rockwellgroup.com", "kanarick");
	radiusSearch.setAuth("brenfer@rockwellgroup.com", "kanarick");
	
	venueDetails.getVenueDetails("101107");
	ofBackground(0,0,0);
	
	//load host + port
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
		}settings.popTag();
	};
	
	cout << "setting up sender at "<<host<<","<<port<<endl;
	
	//setup OSC
	sender.setup(host, port);	
}

//--------------------------------------------------------------
void testApp::update(){
	if ((ofGetElapsedTimeMillis() - lastTimeChecked) > CHECK_TIME*1000){
		
		//set lat and long to the venue you queried
		queryLat = venueDetails.getCurrentVenue()->geoLat;
		queryLong = venueDetails.getCurrentVenue()->geoLong;
		
		//rockwell group, NY
		venueDetails.getVenueDetails("101107");
		radiusSearch.getVenues(queryLat,queryLong, 50);
		lastTimeChecked = ofGetElapsedTimeMillis();
		bChecked = true;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	int curY = 45;
	if (venueDetails.isReady()){
		string newCheckins = ofToString(venueDetails.getCurrentVenue()->getHereNow());
		if (venueDetails.getCurrentVenue()->getNewCheckins() > 0){
			ofSetColor(255,255,0);
			cout << "new checkin! at "<<venueDetails.getCurrentVenue()->getName()<<endl;
			ofDrawBitmapString("There are "+newCheckins+" people checked into "+venueDetails.getCurrentVenue()->getName()+". NEW CHECKIN!", 20, 30);
			
			ofxOscMessage m;
			m.setAddress("/pluginplay/foursquare");
			m.addStringArg(venueDetails.getCurrentVenue()->getName());
			m.addIntArg(venueDetails.getCurrentVenue()->getHereNow());
			sender.sendMessage(m);
			
		} else {
			ofSetColor(255,255,255);
			ofDrawBitmapString("There are "+newCheckins+" people checked into "+venueDetails.getCurrentVenue()->getName(), 20, 30);
		}
		vector<FoursquareUser *> * users = venueDetails.getCurrentVenue()->getUsers();
						
		//trace people checked in
		for (int i=0; i< (*users).size(); i++){
			ofDrawBitmapString((*users)[i]->getName(), 20, curY);
			curY += 15;
		};
		curY += 15;
	}
	
	if (radiusSearch.isReady()){
		
		vector<FoursquareVenue *> * venues = radiusSearch.getCurrentVenues();
		
		for (int i=0; i< venues->size(); i++){
			if ((*venues)[i]->getNewCheckins() > 0){
				cout << "new checkin! at "<<(*venues)[i]->getName()<<endl;
				ofSetColor(255,255,0);
				ofDrawBitmapString("There are "+ofToString((*venues)[i]->getHereNow())+" people checked into "+(*venues)[i]->getName()+" nearby. NEW CHECKIN!", 20, curY);
				
				ofxOscMessage m;
				m.setAddress("/pluginplay/foursquare/nearby");
				m.addStringArg((*venues)[i]->getName());
				m.addIntArg((*venues)[i]->getHereNow());
				sender.sendMessage(m);
			} else {
				ofSetColor(255,255,255);
				ofDrawBitmapString("There are "+ofToString((*venues)[i]->getHereNow())+" people checked into "+(*venues)[i]->getName()+" nearby.", 20, curY);
			}
			curY += 15;
		}
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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


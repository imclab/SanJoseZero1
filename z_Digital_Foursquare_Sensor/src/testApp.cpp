#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
		
	lastTimeChecked = 0;
	bChecked = false;
	bNewCheckin = false;
	bNewCheckinNearby = false;
	
	venueDetails.setAuth("brenfer@rockwellgroup.com", "kanarick");
	radiusSearch.setAuth("brenfer@rockwellgroup.com", "kanarick");
	
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
			venueId = settings.getValue("venueId","101107");
		}settings.popTag();
	};
	
	cout << "setting up sender at "<<host<<","<<port<<endl;
	
	venueDetails.getVenueDetails(venueId);
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
		venueDetails.getVenueDetails(venueId);
		radiusSearch.getVenues(queryLat,queryLong, 50);
		lastTimeChecked = ofGetElapsedTimeMillis();
		bChecked = true;
		bNewCheckin = false;
		bNewCheckinNearby = false;
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	int curY = 45;
	if (venueDetails.isReady()){
		string newCheckins = ofToString(venueDetails.getCurrentVenue()->getHereNow());
		if (venueDetails.getCurrentVenue()->getNewCheckins() > 0){
			ofSetColor(255,255,0);
			
			ofxOscMessage m;
			m.setAddress("/pluginplay/foursquare");
			m.addStringArg(venueDetails.getCurrentVenue()->getName());
			m.addIntArg(venueDetails.getCurrentVenue()->getHereNow());
			sender.sendMessage(m);
			bNewCheckin = true;
		} 
		if (bNewCheckin){			
			cout << "new checkin! at "<<venueDetails.getCurrentVenue()->getName()<<endl;
			ofDrawBitmapString("There are "+newCheckins+" people checked into "+venueDetails.getCurrentVenue()->getName()+". NEW CHECKIN!", 20, 30);
		} else {
			ofSetColor(255,255,255);
			ofDrawBitmapString("There are "+newCheckins+" people checked into "+venueDetails.getCurrentVenue()->getName(), 20, 30);
		}
		vector<FoursquareUser *> * users = venueDetails.getCurrentVenue()->getUsers();
						
		//trace people checked in
		
		if ((*users).size() > 0) ofDrawBitmapString((*users)[0]->getName(), 20, curY);
		
		//for (int i=0; i< (*users).size(); i++){
//			ofDrawBitmapString((*users)[i]->getName(), 20, curY);
//			curY += 15;
//		};
		curY += 15;
	}
	
	if (radiusSearch.isReady()){
		
		vector<FoursquareVenue *> * venues = radiusSearch.getCurrentVenues();
		
		for (int i=0; i< venues->size(); i++){
			if ((*venues)[i]->getNewCheckins() > 0){
				bNewCheckinNearby = true;
				
				ofxOscMessage m;
				m.setAddress("/pluginplay/foursquare/nearby");
				m.addStringArg((*venues)[i]->getName());
				m.addIntArg((*venues)[i]->getHereNow());
				sender.sendMessage(m);
			}
			if (bNewCheckinNearby && (*venues)[i]->getHereNow() > 0){
				ofDrawBitmapString("There are "+ofToString((*venues)[i]->getHereNow())+" people checked into "+(*venues)[i]->getName()+" nearby.", 20, curY);
				curY += 15;
			}
			
			// else {
//				ofSetColor(255,255,255);
//				ofDrawBitmapString("There are "+ofToString((*venues)[i]->getHereNow())+" people checked into "+(*venues)[i]->getName()+" nearby.", 20, curY);
//			}
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
	ofxOscMessage m;
	m.setAddress("/pluginplay/foursquare");
	m.addStringArg(venueDetails.getCurrentVenue()->getName());
	m.addIntArg(venueDetails.getCurrentVenue()->getHereNow());
	sender.sendMessage(m);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


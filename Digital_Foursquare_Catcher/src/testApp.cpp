#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	ofSetLogLevel(OF_LOG_WARNING);
	string searchUrl = "http://www.plug-in-play.com/services/foursquare/rl_foursquare.php";
	
	string lastTimeStamp = "";
	string lastNearbyTimestamp = "";
	
	// SETUP OSC SENDER
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	
	venueId = "61626";
	
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
			
			//php settings
			searchUrl = settings.getValue("searchUrl", "http://www.plug-in-play.com/services/foursquare/rl_foursquare.php");
			
			//in case we need to reset message names 
			settings.pushTag("messages");{
				for (int i=0; i<settings.getNumTags("message"); i++){
					string message = settings.getValue("message", "/pluginplay/foursquare", i);
					messageStrings.push_back(message);
				}
			} settings.popTag();
			
			//get last IDs sent
			settings.pushTag("search");{
				lastTimeStamp = settings.getValue("lastTimeStamp", "");
				lastNearbyTimestamp = settings.getValue("lastNearbyTimestamp", "");
			} settings.popTag();
			
			//get venue Id: just one right now, but leave in framework to get
			// more than one if that ever becomes possible
			settings.pushTag("venues");
			for (int i=0; i<settings.getNumTags("venue"); i++){
				venueId = settings.getValue("venue", "61626");
			}
			settings.popTag();
			
			//console logging
			logLevel = settings.getValue("logLevel", OF_LOG_WARNING );
			
			//how often we search
			searchTime = settings.getValue("searchTime", 6000 );
			nearbySearchTime = settings.getValue("nearbySearchTime", 20000 );
			sendTime = settings.getValue("sendTime", 2000 );
			
		}settings.popTag();
	} else {
		ofLog(OF_LOG_WARNING, "did not find xml");
	}
	
	ofSetLogLevel(logLevel);
	
	//setup OSC
	sender.setup(host, port);
	
	string message = "/pluginplay/foursquare";
	if (messageStrings.size() > 0) message = messageStrings[0];
		
	checkinSearcher = new FoursquareSearcher( "venue_"+venueId, message, searchUrl, &sender, searchTime );
	checkinSearcher->venueId = venueId;
	checkinSearcher->lastID = lastTimeStamp;
	
	nearbySearcher = new FoursquareSearcher( "nearby_"+venueId, message, searchUrl, &sender, nearbySearchTime );
	nearbySearcher->venueId = venueId;
	nearbySearcher->lastID = lastNearbyTimestamp; 
	
	// Initialize the time variables
	lastForwardTime = ofGetElapsedTimeMillis();
	lastHashtagsUpdateTime = ofGetElapsedTimeMillis();
	
	ofSetFrameRate(60);
	//load display font
	font.loadFont("fonts/futura_bold.ttf", 40);
	bSendNearbyResults = false;
}

//--------------------------------------------------------------
void testApp::update(){		
	int curTime = ofGetElapsedTimeMillis();
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 4)+" fps");
	
	// Update the hashtags to search for in case they've changed
	if (curTime - lastHashtagsUpdateTime >= UPDATE_HASHTAGS_SECONDS) {
		lastHashtagsUpdateTime = curTime;
		
		ofxXmlSettings settings;
		bool bLoaded = settings.loadFile("settings.xml");
		if (bLoaded){
			settings.pushTag("settings");
			//how often we search
			searchTime = settings.getValue("searchTime", 6000 );
			sendTime = settings.getValue("sendTime", 2000 );
			
			settings.pushTag("venues");
			for (int i=0; i<settings.getNumTags("venue"); i++){
				venueId = settings.getValue("venue", "61626");
			}
			settings.popTag();
			checkinSearcher->venueId = venueId;
			checkinSearcher->name="venue_"+venueId;
			
			checkinSearcher->setSearchTime(searchTime);
			settings.popTag();
		}		
		saveSettings();
	}
		
	// Update the XML file from the PHP script if enough time has elapsed
	
	if ( checkinSearcher->bReadyToSearch() ) 
		checkinSearcher->doSearch();
	
	if ( nearbySearcher->bReadyToSearch() ) 
		nearbySearcher->doSearch();

	
	if (ofGetElapsedTimeMillis() - lastForwardTime >= sendTime) {
		lastForwardTime = ofGetElapsedTimeMillis();
		if (bSendNearbyResults && nearbySearcher->hasResults())
			nearbySearcher->sendOSCSetup();
		else 
			checkinSearcher->sendOSCSetup();
		
		bSendNearbyResults = !bSendNearbyResults;
	}
		
}
	
//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(150,107,181);
	font.drawString("FOURSQUARE", 20, 60);
	ofSetColor(0xffffff);	
	checkinSearcher->draw(20,100);
	nearbySearcher->draw(300, 100);
}

//--------------------------------------------------------------
// SAVE OUT NEWEST ID IN CASE OF CRASH / RESTART 
void testApp::saveSettings(){
	
	ofxXmlSettings settings;
	
	//load up old settings so you don't write over osc, etc
	// + save things like log level + timing to make sure we see them
	// in the xml
	
	settings.loadFile("settings.xml");
	settings.setValue("settings:search:lastTimeStamp", checkinSearcher->lastID);
	settings.setValue("settings:search:lastNearbyTimestamp", nearbySearcher->lastID);
	settings.setValue("settings:logLevel", logLevel);
	settings.setValue("settings:searchTime", searchTime );
	settings.setValue("settings:nearbySearchTime", nearbySearchTime );
	settings.setValue("settings:sendTime", sendTime );
	settings.saveFile("settings.xml");
};

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


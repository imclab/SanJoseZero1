#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	ofSetLogLevel(OF_LOG_WARNING);
	string searchUrl = "http://plug-in-play.com/services/flickr/rl_flickr.php";
	
	string lastTagId = "";
	string lastNearbyId = "";

	//geo params
	string radius = "10";
	string lat = "40.735844095042395";
	string longitude = "-73.9904522895813";
	
	//tags
	vector<string>tags;
	
	// SETUP OSC SENDER
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
			
			//php settings
			searchUrl = settings.getValue("searchUrl", searchUrl);
			
			//in case we need to reset message names 
			settings.pushTag("messages");{
				for (int i=0; i<settings.getNumTags("message"); i++){
					string message = settings.getValue("message", "/pluginplay/twitter", i);
					messageStrings.push_back(message);
				}
			} settings.popTag();
			
			//get last IDs sent
			settings.pushTag("search");{
				lastNearbyId = settings.getValue("lastNearbyId", "");
				lastTagId = settings.getValue("lastTagId", "");
			} settings.popTag();
			
			//console logging
			logLevel = settings.getValue("logLevel", OF_LOG_WARNING );
			
			//how often we search
			searchTime = settings.getValue("searchTime", (float) SEARCH_TIME_SECONDS );
			sendTime = settings.getValue("sendTime", 2000 );
			cout<<"send time is "<<sendTime<<endl;
			
			//get geo params
			radius = settings.getValue("geo:radius", radius);
			lat = settings.getValue("geo:lat", lat);
			longitude = settings.getValue("geo:long", longitude);
			
			//get tags to search
			settings.pushTag("tags");{
				for (int i=0; i<settings.getNumTags("tag"); i++){
					string tag = settings.getValue("tag", "",i);
					tags.push_back(tag);
				}
			} settings.popTag();
			
		}settings.popTag();
	} else {
		ofLog(OF_LOG_WARNING, "did not find xml");
	}
	
	ofSetLogLevel(logLevel);
	
	//setup OSC
	sender.setup(host, port);
	
	string atMessage = "/pluginplay/twitter";
	string tagMessage = "/pluginplay/twitter";
	if (messageStrings.size() > 0) atMessage = messageStrings[0];
	if (messageStrings.size() > 1) tagMessage = messageStrings[1];
		
	nearbySearcher = new FlickrSearcher( "atSearch", atMessage, searchUrl, &sender, searchTime );
	nearbySearcher->lastID = lastNearbyId;
	nearbySearcher->setGeoParams(radius, lat, longitude);
	
	tagSearcher = new FlickrSearcher( "tagSearch", tagMessage, searchUrl, &sender, searchTime );
	tagSearcher->lastID = lastTagId;
	
	//loop through tags vector and add to searcher
	for (int i=0; i<tags.size(); i++){
		tagSearcher->addTag(tags[i]);
	}
	
	// Initialize the time variables
	lastForwardTime = ofGetElapsedTimeMillis();
	lastHashtagsUpdateTime = ofGetElapsedTimeMillis();
	
	ofLog(OF_LOG_VERBOSE,  "num of tags: "+ofToString(tagSearcher->getNumTags()));
	ofSetFrameRate(60);
	
	//load display font
	font.loadFont("fonts/futura_bold.ttf", 40);
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
		
		if (bLoaded) {
			tagSearcher->clearTags(); 
			settings.pushTag("settings");{
				
				settings.pushTag("tags");{
					for (int i = 0; i < settings.getNumTags("tag"); i++) {
						tagSearcher->addTag(settings.getValue("tag","void",i));
					}
				} settings.popTag();
				
				//console logging
				logLevel = settings.getValue("logLevel", OF_LOG_WARNING );
								
				//search + send time
				searchTime = settings.getValue("searchTime", (float) SEARCH_TIME_SECONDS );
				sendTime = settings.getValue("sendTime", 2000 );
				
				nearbySearcher->setSearchTime(searchTime);
				tagSearcher->setSearchTime(searchTime);
				
				//geo params
				string radius = settings.getValue("geo:radius", radius);
				string lat = settings.getValue("geo:lat", lat);
				string longitude = settings.getValue("geo:long", longitude);
				
				nearbySearcher->setGeoParams(radius, lat, longitude);
				
			} settings.popTag();
		}		
		saveSettings();
	}
	
	
	// Update the XML file from the PHP script if enough time has elapsed
	
	if ( tagSearcher->bReadyToSearch() ) 
		tagSearcher->doSearch();

	if ( nearbySearcher->bReadyToSearch() ) 
		nearbySearcher->doSearch();
	
	if (ofGetElapsedTimeMillis() - lastForwardTime >= sendTime) {
		lastForwardTime = ofGetElapsedTimeMillis();
		tagSearcher->sendOSCSetup();
		nearbySearcher->sendOSCSetup();
	}
		
}
	
//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255,0,132);
	font.drawString("FLICKR", 20, 60);
	ofSetColor(0xffffff);
	nearbySearcher->draw(20,100);
	tagSearcher->draw(400,100);
	
	//ofDrawBitmapString(newestTweet, 10, 10);
}

//--------------------------------------------------------------
// SAVE OUT NEWEST ID IN CASE OF CRASH / RESTART 
void testApp::saveSettings(){
	
	ofxXmlSettings settings;
	
	//load up old settings so you don't write over osc, etc
	// + save things like log level + timing to make sure we see them
	// in the xml
	
	settings.loadFile("settings.xml");
	settings.setValue("settings:search:lastTagId", tagSearcher->lastID);
	settings.setValue("settings:search:lastNearbyId", nearbySearcher->lastID);
	settings.setValue("settings:logLevel", logLevel);
	settings.setValue("settings:searchTime", searchTime );
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


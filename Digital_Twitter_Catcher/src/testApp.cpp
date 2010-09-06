#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	ofSetLogLevel(OF_LOG_WARNING);
	string searchUrl = "http://plug-in-play.com/services/twitter/rl_twitter.php";
	
	string lastHashID = "";
	string lastReplyID = "";
	
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
			searchUrl = settings.getValue("searchUrl", "http://plug-in-play.com/services/twitter/rl_twitter.php");
			
			//in case we need to reset message names 
			settings.pushTag("messages");{
				for (int i=0; i<settings.getNumTags("message"); i++){
					string message = settings.getValue("message", "/pluginplay/twitter", i);
					messageStrings.push_back(message);
				}
			} settings.popTag();
			
			//get last IDs sent
			settings.pushTag("search");{
				lastHashID = settings.getValue("lastHashID", "");
				lastReplyID = settings.getValue("lastReplyID", "");
			} settings.popTag();
			
			//console logging
			logLevel = settings.getValue("logLevel", OF_LOG_WARNING );
			
			//how often we search
			searchTime = settings.getValue("searchTime", (float) SEARCH_TIME_SECONDS );
			sendTime = settings.getValue("sendTime", 2000 );
			
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
	
	atSearcher = new TwitterSearcher( "atSearch", atMessage, searchUrl, &sender, searchTime );
	atSearcher->lastID = lastReplyID;
	
	tagSearcher = new TwitterSearcher( "tagSearch", tagMessage, searchUrl, &sender, searchTime );
	tagSearcher->lastID = lastHashID;
	
	
	// Initialize the time variables
	lastForwardTime = ofGetElapsedTimeMillis();
	lastHashtagsUpdateTime = ofGetElapsedTimeMillis();
	
	// Update the hashtags to search for
	ofxXmlSettings hTags;
	bLoaded = hTags.loadFile("hashtags.xml");
	if (bLoaded) {
		hTags.pushTag("settings");
		
//		curNumOfTags = hTags.getNumTags("tag");
		for (int i = 0; i < hTags.getNumTags("tag"); i++) {
			tagSearcher->addTag(hTags.getValue("tag","void",i));
			//hashTags.push_back(hTags.getValue("tag","void",i));
		}
		hTags.popTag();
	}
	
	ofLog(OF_LOG_VERBOSE,  "num of tags: "+ofToString(tagSearcher->getNumTags()));
	ofSetFrameRate(60);
	
	//load display font
	font.loadFont("fonts/futura_bold.ttf", 40);
	bSendAtReplies = true;
}

//--------------------------------------------------------------
void testApp::update(){		
	int curTime = ofGetElapsedTimeMillis();
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 4)+" fps");
	
	// Update the hashtags to search for in case they've changed
	if (curTime - lastHashtagsUpdateTime >= UPDATE_HASHTAGS_MILLIS) {
		lastHashtagsUpdateTime = curTime;
		ofxXmlSettings hTags;
		bool bLoaded = hTags.loadFile("hashtags.xml");
		if (bLoaded) {
			tagSearcher->clearTags(); 
			hTags.pushTag("settings");{
				
				for (int i = 0; i < hTags.getNumTags("tag"); i++) {
					tagSearcher->addTag(hTags.getValue("tag","void",i));
				}
				
				//console logging
				logLevel = hTags.getValue("logLevel", OF_LOG_WARNING );
			} hTags.popTag();
		} else {
		}
		
		ofxXmlSettings settings;
		bLoaded = settings.loadFile("settings.xml");
		if (bLoaded){
			settings.pushTag("settings");{
				//how often we search
				searchTime = settings.getValue("searchTime", (float) SEARCH_TIME_SECONDS );
				sendTime = settings.getValue("sendTime", 2000 );
				
				atSearcher->setSearchTime(searchTime);
				tagSearcher->setSearchTime(searchTime);
			} settings.popTag();
		}	
		saveSettings();	
	}
	
	
	// Update the XML file from the PHP script if enough time has elapsed
	
	if ( tagSearcher->bReadyToSearch() ) 
		tagSearcher->doSearch();

	if ( atSearcher->bReadyToSearch() ) 
		atSearcher->doSearch();
	
	if (ofGetElapsedTimeMillis() - lastForwardTime >= sendTime) {
		lastForwardTime = ofGetElapsedTimeMillis();
		if (bSendAtReplies && atSearcher->hasWaitingResults())
			atSearcher->sendOSCSetup();
		else			
			tagSearcher->sendOSCSetup();
		
		bSendAtReplies = !bSendAtReplies;
	}
}
	
//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(50,204,255);
	font.drawString("TWITTER", 20, 60);
	ofSetColor(0xffffff);
	atSearcher->draw(20,100);
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
	settings.setValue("settings:search:lastHashID", tagSearcher->lastID);
	settings.setValue("settings:search:lastReplyID", atSearcher->lastID);
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


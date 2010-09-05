#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetLogLevel(OF_LOG_WARNING);
	
	string searchUrl = "http://plug-in-play.com/services/twitter/rl_twitter.php";
	
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
			
		}settings.popTag();
	} else {
		ofLog(OF_LOG_WARNING, "did not find xml");
	}
	
	//setup OSC
	sender.setup(host, port);
	
	string atMessage = "/pluginplay/twitter";
	string tagMessage = "/pluginplay/twitter";
	if (messageStrings.size() > 0) atMessage = messageStrings[0];
	if (messageStrings.size() > 1) tagMessage = messageStrings[1];
	
	cout<<messageStrings.size()<<":"<<atMessage<<":"<<tagMessage<<endl;
	
	atSearcher = new TwitterSearcher( "atSearch", atMessage, searchUrl, &sender );
	tagSearcher = new TwitterSearcher( "tagSearch", tagMessage, searchUrl, &sender );
	
	
	// Initialize the time variables
	time_t curTime;
	time(&curTime);
	lastForwardTime = curTime;
	lastHashtagsUpdateTime = curTime;
	
	// Update the hashtags to search for
	ofxXmlSettings hTags;
	bLoaded = hTags.loadFile("hashtags.xml");
	if (bLoaded) {
		hTags.pushTag("settings");
		cout << "detected tags: " << hTags.getNumTags("tag");
//		curNumOfTags = hTags.getNumTags("tag");
		for (int i = 0; i < hTags.getNumTags("tag"); i++) {
			tagSearcher->addTag(hTags.getValue("tag","void",i));
			//hashTags.push_back(hTags.getValue("tag","void",i));
		}
		hTags.popTag();
	}
	
	ofLog(OF_LOG_VERBOSE,  "num of tags: "+ofToString(tagSearcher->getNumTags()));
}

//--------------------------------------------------------------
void testApp::update(){	
	time_t curTime;
	time(&curTime);
	
	// Update the hashtags to search for in case they've changed
	if (difftime(curTime,lastHashtagsUpdateTime) >= UPDATE_HASHTAGS_SECONDS) {
		lastHashtagsUpdateTime = curTime;
		ofxXmlSettings hTags;
		bool bLoaded = hTags.loadFile("hashtags.xml");
		if (bLoaded) {
			tagSearcher->clearTags(); 
			hTags.pushTag("settings");
//			curNumOfTags = hTags.getNumTags("tag");
			for (int i = 0; i < hTags.getNumTags("tag"); i++) {
				tagSearcher->addTag(hTags.getValue("tag","void",i));
				//hashTags.push_back(hTags.getValue("tag","void",i));
			}
			hTags.popTag();
		}
	}
	
	
	// Update the XML file from the PHP script if enough time has elapsed
	if (difftime(curTime,lastSearchTime) >= SEARCH_TIME_SECONDS && 
		(tagSearcher->bNewQuery || atSearcher->bNewQuery ) ){
		lastSearchTime = curTime;
		
		// Get more XML from the .php script
		
//FIX! THESE SHOULD BE IN SEPARATE TIMERS
		
		tagSearcher->doSearch();
		atSearcher->doSearch();
	}


	// Send the next OSC message if enough time has elapsed
	
	if (difftime(curTime,lastForwardTime) >= FORWARD_TIME_SECONDS) {
		lastForwardTime = curTime;
		tagSearcher->sendOSCSetup();
		atSearcher->sendOSCSetup();
	}
		
	saveSettings();
}

	
//--------------------------------------------------------------
void testApp::draw(){
	
	if (true){
		ofBackground(0,150,150);
	} else {
		ofBackground(0,0,0);
	}
	//ofDrawBitmapString(newestTweet, 10, 10);
}

//--------------------------------------------------------------
// SAVE OUT NEWEST ID IN CASE OF CRASH / RESTART 
void testApp::saveSettings(){
	
	ofxXmlSettings settings;
	
	//load up old settings so you don't write over osc, etc
	settings.loadFile("settings.xml");
	settings.setValue("settings:search:lastHashID", tagSearcher->lastID);
	settings.setValue("settings:search:lastReplyID", atSearcher->lastID);
	settings.saveFile("settings.xml");
	
	tagSearcher->saveSettings();
	atSearcher->saveSettings();
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


#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	ofSetLogLevel(OF_LOG_WARNING);
	string searchUrl = "http://plug-in-play.com/services/twitter/rl_twitter.php";
	
	string lastHashID = "";
	string lastReplyID = "";
	
	lastSent = 0;
	
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
						
			//get messages + data for each sender
			settings.pushTag("messages");{
				for (int i=0; i<settings.getNumTags("message"); i++){
					settings.pushTag("message", i);{
						int type = settings.getValue("type", 1);
						
						string message = settings.getValue("string", "/pluginplay/twitter");
						float searchTime = settings.getValue("time", (float) SEARCH_TIME_SECONDS);
						string lastId = settings.getValue("lastId", lastId);
						
						if (type == 0){					
							TwitterSearcher * atSearcher = new TwitterSearcher( "atSearch", message, searchUrl, &sender, searchTime );
							atSearcher->lastID = lastId;
							searchers.push_back(atSearcher);
						} else {
							TwitterSearcher * tagSearcher = new TwitterSearcher( "tagSearch "+i, message, searchUrl, &sender, searchTime );
							
							// add tags (doing it this way = multiple tags per sender if you want)
							for (int j=0; j<settings.getNumTags("tag"); j++){
								string tag = settings.getValue("tag", "", j);
								tagSearcher->addTag(tag);
							}
							tagSearcher->lastID = lastId;
							searchers.push_back(tagSearcher);
						}
					} settings.popTag();
				}
			} settings.popTag();
						
			//console logging
			logLevel = settings.getValue("logLevel", OF_LOG_WARNING );
			
			//how often we send
			sendTime = settings.getValue("sendTime", 2000 );
			
		}settings.popTag();
	} else {
		ofLog(OF_LOG_WARNING, "did not find xml");
	}
	
	ofSetLogLevel(logLevel);
	
	//setup OSC
	sender.setup(host, port);		
	
	// Initialize the time variables
	lastForwardTime = ofGetElapsedTimeMillis();
	lastHashtagsUpdateTime = ofGetElapsedTimeMillis();
	
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
		
		ofxXmlSettings settings;
		bool bLoaded = settings.loadFile("settings.xml");
		if (bLoaded){
			settings.pushTag("settings");{
				//how often we search
				sendTime = settings.getValue("sendTime", 2000 );
				settings.pushTag("messages");{					
					for (int i=0; i<settings.getNumTags("message"); i++){
						settings.pushTag("message", i);{
							int type = settings.getValue("type", 1);
							
							string message = settings.getValue("string", "/pluginplay/twitter");
							float searchTime = settings.getValue("time", (float) SEARCH_TIME_SECONDS);
							string lastId = settings.getValue("lastId", lastId);
							
							if (type == 0){								
								if (i > searchers.size()){									
									TwitterSearcher * nearbySearcher = new TwitterSearcher( "repySearch", message, searchUrl, &sender, searchTime );
									searchers.push_back(nearbySearcher);
								} else {
									searchers[i]->setSearchTime(searchTime);
								};
							} else {
								
								if (i > searchers.size()){
									TwitterSearcher * tagSearcher = new TwitterSearcher( "tagSearch "+ofToString(i), message, searchUrl, &sender, searchTime );
									
									// add tags (doing it this way = multiple tags per sender if you want)
									for (int i=0; i<settings.getNumTags("tag"); i++){
										string tag = settings.getValue("tag", "", i);
										tagSearcher->addTag(tag);
									}
									searchers.push_back(tagSearcher);
								} else {
									searchers[i]->clearTags();
									for (int j=0; j<settings.getNumTags("tag"); j++){
										string tag = settings.getValue("tag", "", j);
										searchers[i]->addTag(tag);
									}
									searchers[i]->setSearchTime(searchTime);
								};
							}
						} settings.popTag();
					}
				} settings.popTag();;
			} settings.popTag();
		}	
		saveSettings();	
	}
	
	
	// Update the XML file from the PHP script if enough time has elapsed
	
	for (int i=0; i<searchers.size(); i++){
		if (searchers[i]->bReadyToSearch())
			searchers[i]->doSearch();
	};
	
	if (ofGetElapsedTimeMillis() - lastForwardTime >= sendTime) {
		lastForwardTime = ofGetElapsedTimeMillis();
		lastSent++;
		if (lastSent > searchers.size()) lastSent = 0;
		
		bool bSent = false;
		for (int i=lastSent; i<searchers.size(); i++){
			if (searchers[i]->hasWaitingResults()){
				searchers[i]->sendOSCSetup();
				lastSent = i;
				bSent = true;
				break;
			}			
		};
		if (!bSent){
			for (int i=0; i<lastSent; i++){
				if (searchers[i]->hasWaitingResults()){
					searchers[i]->sendOSCSetup();
					lastSent = i;
					break;
				}			
			};
		};
		
		bSendAtReplies = !bSendAtReplies;
	}
}
	
//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(50,204,255);
	font.drawString("TWITTER", 20, 60);
	ofSetColor(0xffffff);
	for (int i=0; i<searchers.size(); i++){
		searchers[i]->draw(20+200*i,100);
	}
	
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
	settings.pushTag("settings");{
		settings.pushTag("messages");{
			for (int i=0; i<searchers.size(); i++){
				settings.pushTag("message", i);{				
					settings.setValue("lastId", searchers[i]->lastID);
				} settings.popTag();
			}
		} settings.popTag();
	} settings.popTag();	
	settings.setValue("settings:logLevel", logLevel);
	settings.setValue("settings:searchTime", searchTime );
	settings.setValue("settings:replySearchTime", replySearchTime );
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


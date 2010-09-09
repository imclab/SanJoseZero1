#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	ofSetLogLevel(OF_LOG_WARNING);
	string searchUrl = "http://plug-in-play.com/services/flickr/rl_flickr.php";
	
	//geo params
	string radius = "10";
	string lat = "40.735844095042395";
	string longitude = "-73.9904522895813";
	
	//tags
	vector<string>tags;
	lastSent = 0;
		
	// SETUP OSC SENDER
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings/settings.xml");
	string host = "localhost";
	int port = 12345;
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
			
			//php settings
			searchUrl = settings.getValue("searchUrl", searchUrl);
			
			//get messages + data for each sender
			settings.pushTag("messages");{
				for (int i=0; i<settings.getNumTags("message"); i++){
					settings.pushTag("message", i);{
						int type = settings.getValue("type", 1);
						
						string message = settings.getValue("string", "/pluginplay/flickr");
						float searchTime = settings.getValue("time", (float) SEARCH_TIME_SECONDS);
						string lastId = settings.getValue("lastId", lastId);
						
						if (type == 0){
							//get geo params
							radius = settings.getValue("geo:radius", radius);
							lat = settings.getValue("geo:lat", lat);
							longitude = settings.getValue("geo:long", longitude);
							ofLog(OF_LOG_VERBOSE, "Geo search: "+lat+", "+longitude+", "+radius);
							
							FlickrSearcher * nearbySearcher = new FlickrSearcher( "nearbySearch", message, searchUrl, &sender, searchTime );
							nearbySearcher->lastID = lastId;
							nearbySearcher->setGeoParams(radius, lat, longitude);
							searchers.push_back(nearbySearcher);
						} else {
							FlickrSearcher * tagSearcher = new FlickrSearcher( "tagSearch "+i, message, searchUrl, &sender, searchTime );
							
							// add tags (doing it this way = multiple tags per sender if you want)
							for (int j=0; j<settings.getNumTags("tag"); j++){
								string tag = settings.getValue("tag", "", j);
								tags.push_back(tag);
								tagSearcher->addTag(tag);
								cout<<"add tag "<<tag<<" "<<searchers.size()<<endl;
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
	
	ofLog(OF_LOG_VERBOSE,  "num of tags: "+ofToString((int)tags.size()));
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
		bool bLoaded = settings.loadFile("settings/settings.xml");
		
		if (bLoaded) {
			settings.pushTag("settings");{
				
				settings.pushTag("messages");{
					
					for (int i=0; i<settings.getNumTags("message"); i++){
						settings.pushTag("message", i);{
							int type = settings.getValue("type", 1);
							
							string message = settings.getValue("string", "/pluginplay/flickr");
							float searchTime = settings.getValue("time", (float) SEARCH_TIME_SECONDS);
							string lastId = settings.getValue("lastId", lastId);
							
							if (type == 0){
								//get geo params
								string radius = settings.getValue("geo:radius", radius);
								string lat = settings.getValue("geo:lat", lat);
								string longitude = settings.getValue("geo:long", longitude);
								ofLog(OF_LOG_VERBOSE, "Geo search: "+(lat)+", "+(longitude)+", "+(radius));
								
								if (i > searchers.size()){									
									FlickrSearcher * nearbySearcher = new FlickrSearcher( "nearbySearch", message, searchUrl, &sender, searchTime );
									nearbySearcher->lastID = lastId;
									nearbySearcher->setGeoParams(radius, lat, longitude);
									searchers.push_back(nearbySearcher);
								} else {
									searchers[i]->setSearchTime(searchTime);
									searchers[i]->lastID = lastId;
									searchers[i]->setGeoParams(radius, lat, longitude);
								};
							} else {
								
								if (i > searchers.size()){
									FlickrSearcher * tagSearcher = new FlickrSearcher( "tagSearch "+ofToString(i), message, searchUrl, &sender, searchTime );
									
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
				
				//console logging
				logLevel = settings.getValue("logLevel", OF_LOG_WARNING );
								
				//search + send time
				sendTime = settings.getValue("sendTime", 2000 );
				
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
	}
		
}
	
//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255,0,132);
	font.drawString("FLICKR", 20, 60);
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
	
	settings.loadFile("settings/settings.xml");
	
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
	settings.setValue("settings:sendTime", sendTime );
	settings.saveFile("settings/settings.xml");
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


#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	infoGetter.start();
	
	ofAddListener(infoGetter.newResponseEvent,this,&testApp::infoResponse);
	
	// SETUP OSC SENDER
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
//			searchTerm = settings.getValue("search","play");
		}settings.popTag();
	};
	//setup OSC
	sender.setup(host, port);
	
	
	
	bFirstQuery = true;
	bNewQuery = true;
	bOKtoSend = false;
	maxNumOfTweets = 0;
	curTweet = 0;
	
	// Initialize the time variables
	time_t curTime;
	time(&curTime);
	lastSearchTime = curTime;
	lastForwardTime = curTime;
	lastHashtagsUpdateTime = curTime;
	lastHashID = "";
	lastReplyID = "";

	
	// Update the hashtags to search for
	ofxXmlSettings hTags;
	bLoaded = hTags.loadFile("hashtags.xml");
	if (bLoaded) {
		hTags.pushTag("settings");
		cout << "detected tags: " << hTags.getNumTags("tag");
//		curNumOfTags = hTags.getNumTags("tag");
		for (int i = 0; i < hTags.getNumTags("tag"); i++) {
			hashTags.push_back(hTags.getValue("tag","void",i));
		}
		hTags.popTag();
	}
	
	
	cout << "num of tags: " << hashTags.size() << endl;
	for (int i = 0; i < hashTags.size(); i++) {
		cout << "tag: " << hashTags[i] << endl;
	}
	
}

//----------------------------------------------------------------
void testApp::doSearch() {
//chrisallick.com/projects/twitter/rl_twitter.php?hash=0
	
	ofxHttpForm form;
	form.action = "http://chrisallick.com/projects/twitter/rl_twitter.php";  //"http:www.plug-in-play.com/testing5.php";
	form.method = OFX_HTTP_POST;


	// Get @replies
	form.addFormField("hash","0");
	if (lastReplyID != "") {
		cout << "adding reply id: " << lastReplyID << endl;
		form.addFormField("ms",lastReplyID);
	}
	cout << "Getting @replies....\n";
	infoGetter.addForm(form);

	
	// Get hashtags
	form.clearFormFields();
	form.addFormField("hash","1");
	if (lastHashID != "") {
		cout << "adding hash id: " << lastHashID << endl;		
		form.addFormField("ms",lastHashID);
	}
	// ADD TAGS
	for (int i = 0; i < hashTags.size(); i++) {
		form.addFormField("tag[" + ofToString(i) + "]",hashTags[i]);
	}
//	form.addFormField("ms","22734030265");
	cout << "Getting hashtags....\n";
	infoGetter.addForm(form);
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
			hashTags.clear();
			hTags.pushTag("settings");
//			curNumOfTags = hTags.getNumTags("tag");
			for (int i = 0; i < hTags.getNumTags("tag"); i++) {
				hashTags.push_back(hTags.getValue("tag","void",i));
			}
			hTags.popTag();
		}
	}
	
	
	// Update the XML file from the PHP script if enough time has elapsed
	if (difftime(curTime,lastSearchTime) >= SEARCH_TIME_SECONDS && bNewQuery){
		bNewQuery = false;
		lastSearchTime = curTime;
		// Get more XML from the .php script
		doSearch();
	}


	// Send the next OSC message if enough time has elapsed
	if (difftime(curTime,lastForwardTime) >= FORWARD_TIME_SECONDS && bOKtoSend) {
		lastForwardTime = curTime;
		sendOSCSetup();
	}
		
		
}

//--------------------------------------------------------------
bool testApp::getNextResponseSet() {
	if (responses.size() > 1) {
		// Remove oldest entry
		responses.erase(responses.begin());
		
		xmlResponse.loadFromBuffer((responses[0]).responseBody);
		xmlResponse.pushTag("results");
		maxNumOfTweets = xmlResponse.getNumTags("tweet");
		xmlResponse.popTag();
		
		curTweet = 0;
		bOKtoSend = true;
		return true;
	} else
		return false;
}


//--------------------------------------------------------------
void testApp::infoResponse(ofxHttpResponse &response) {
	xmlResponse.loadFromBuffer(response.responseBody);

	
	cout << "response:\n " << response.responseBody << endl;

	xmlResponse.pushTag("results");
	cout << "attribute: " << xmlResponse.getAttribute("tweet","type","void");
	xmlResponse.popTag();
	cout << endl << endl << endl;
	
	
	
	
	
	
	// ERROR CHECKING: WHAT IF RETURNED NOTHING?  NOTHING NEW.
	
	
	if (!responses.empty()) {
		responses.push_back(response);
	} else {
	
		xmlResponse.pushTag("results");
		maxNumOfTweets = xmlResponse.getNumTags("tweet");
		xmlResponse.popTag();
		
		curTweet = 0;
		if (curTweet < maxNumOfTweets) {
			bOKtoSend = true;
		} else {
			bOKtoSend = false;
			bNewQuery = true;
		}
	}
	
}

//--------------------------------------------------------------
void testApp::sendOSCSetup() {
	if (curTweet == maxNumOfTweets) {
		// SEE IF WE NEED TO GET ANOTHER RESPONSE SET FROM responses VECTOR
		if (getNextResponseSet()) {
			sendOSC();
		} else {
			bNewQuery = true;
			bOKtoSend = false;
		}
	} else {
		sendOSC();
	}

}

//--------------------------------------------------------------
void testApp::sendOSC() {
	
	xmlResponse.pushTag("results");
	string type_of_result = xmlResponse.getAttribute("tweet","type","",curTweet);
	xmlResponse.pushTag("tweet",curTweet);
	
	ofxOscMessage m;
	m.setAddress("/pluginplay/twitter");
	m.addStringArg(xmlResponse.getValue("timestamp","null") + "&*!" + xmlResponse.getValue("user_name","null") + "&*!" + xmlResponse.getValue("content","null"));
	sender.sendMessage(m);
	

	// Change the last twitter id stamp
	cout << type_of_result << endl;
	if (type_of_result == "hash") {
		lastHashID = xmlResponse.getValue("tweet_id","");
		cout << "setting last hash: " << lastHashID << endl;
	} else if (type_of_result == "reply") {
		lastReplyID = xmlResponse.getValue("tweet_id","");
		cout << "setting last reply: " << lastReplyID << endl;		
	}
	
	
	xmlResponse.popTag();
	xmlResponse.popTag();
	
	curTweet++;
}
	
//--------------------------------------------------------------
void testApp::draw(){
	if (true){
		ofBackground(0,150,150);
	} else {
		ofBackground(0,0,0);
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


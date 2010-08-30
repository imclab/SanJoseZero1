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
	curTimestamp = "";

	
	
}

//----------------------------------------------------------------
void testApp::doSearch() {
	ofxHttpForm form;
	form.action = "http://www.plug-in-play.com/testing5.php";
	form.method = OFX_HTTP_POST;
	
	cout << "Getting....\n";
	infoGetter.addForm(form);
}



//--------------------------------------------------------------
void testApp::update(){	
	time_t curTime;
	time(&curTime);
	
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
		sendOSC();
	}
		
		
}

//--------------------------------------------------------------
void testApp::infoResponse(ofxHttpResponse &response) {
	xmlResponse.loadFromBuffer(response.responseBody);
	
//	cout << "response:\n " << response.responseBody << endl;

	// ERROR CHECKING: WHAT IF RETURNED NOTHING?  NOTHING NEW.  MUST AVOID THE FIRST if STATEMENT IN sendOSC()
	
	xmlResponse.pushTag("settings");
	maxNumOfTweets = xmlResponse.getNumTags("tweet");
	curTweet = 0;
	bOKtoSend = true;
//	for (int i = 0; i < xmlResponse.getNumTags("tweets"); i++) {
//		xmlResponse.pushTag("tweets",i);
//		cout << "tag: " << i << ", contents: " << xmlResponse.getValue("content","nothin'",0) << endl;
//		xmlResponse.popTag();
//		
//	}
//	xmlResponse.popTag();
//	newQuery = true;


}

//--------------------------------------------------------------
void testApp::sendOSC() {
	if (curTweet == maxNumOfTweets) {
		xmlResponse.popTag();  // pop the "settings" tag
		bNewQuery = true;
		bOKtoSend = false;
		cout << endl;
		// Write current timestamp to file
//		cout << "opening\n";
//		cout << "current path:\n";
//		char CurrentPath[100];
//		getcwd(CurrentPath,100);
//		cout << CurrentPath << endl;
//		cout << ofToDataPath("untitled.txt",false);
//		
		return;
	}

	xmlResponse.pushTag("tweet",curTweet++);
	cout << xmlResponse.getValue("content","void",0) << endl;
	curTimestamp = xmlResponse.getValue("timestamp","void",0);
	xmlResponse.popTag();

/*	
	ofxOxcMessage m;
	m.setAddress("/pluginplay/twitter");
	m.addStringArg(xmlResponse.getValue("content","void",0));
	sender.sendMessage(m);
	xmlResponse.popTag();
*/
}

//--------------------------------------------------------------
void testApp::draw(){
	if (true){
		ofBackground(0,150,150);
	} else {
		ofBackground(0,0,0);
	}
//	cout<<"here?"<<endl;
//	ofDrawBitmapString("newest tweet from: "+latestSearchEntry.author.name+"\n"+latestSearchEntry.entry+"\nat "+latestSearchEntry.published, 20, 30);
}

//--------------------------------------------------------------
void testApp::exit() {
//	FILE *f;
//	f = fopen("../../../data/lastTimestamp.txt","w"); // Don't truncate in case the app is closed during file operations --> this way we won't lose any data.
//	cout << "putting\n";
//	fputs(curTimestamp.c_str(),f);
//	cout << "closing\n";
//	fclose(f);	
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


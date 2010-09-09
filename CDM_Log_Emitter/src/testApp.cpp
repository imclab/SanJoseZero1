#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(256,256,150);
	
	// Initialize variables
	curTotalEmissions = 0;
	curEmission = 0;
	nextEmitInterval = 0;
	lastEmitTimeMillis = ofGetElapsedTimeMillis();
	bRequestingXML = false;
	bOKtoGetNextChunk = true;
	bFirstTimeThrough = true;
	bNoMoreXML = false;
	LOG = "";
	
	
	
	// Set up OSC
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	messageAddress = "/pluginplay/playground";
	
	settings.pushTag("settings"); {
		host = settings.getValue("host",host);
		port = settings.getValue("port",port);
		messageAddress = settings.getValue("messageAddress",messageAddress);
	} settings.popTag();
	
	OSCsender.setup(host,port);
	
	
	
	// Set up Http communications
	// Initialize variables
	highestEmitID = -1;
	postURL = "http://www.plug-in-play.com/php/cdmLogger.php";
	sDate = "";
	// Set up the sDate variable for Http communications
	int month = ofGetMonth();
	int day = ofGetDay();
	// Set the sDate variable accordingly
	if (month < 10) {
		sDate = "0";
	}
	sDate += ofToString(month);
	if (day < 10) {
		sDate += "0";
	}
	sDate += ofToString(day);

	// Set up the communications
	xmlGetter.start();
	ofAddListener(xmlGetter.newResponseEvent,this,&testApp::receivedNextXMLchunk);
	
	cout << "end of setup\n";
}


//--------------------------------------------------------------
void testApp::update(){
	
	// Is it time to load the next chunk of XML?
	if (curEmission == curTotalEmissions) {

		// Remove the current XML chunk only if it's not the first
		//   time through or if there is more XML to parse
		if (!bFirstTimeThrough && !bNoMoreXML) {
			sResultsQueue.pop_front();
		}
		
		if (!sResultsQueue.empty()) {

			// Read in next XML chunk in order to set variables
			ofxXmlSettings curChunk;
			string sCurChunk;
			sCurChunk = sResultsQueue.front();
			curChunk.loadFromBuffer(sCurChunk);
			
			curChunk.pushTag("results"); {
				curTotalEmissions = curChunk.getNumTags("emission");
			} curChunk.popTag();
			curChunk.clear();
			
			
			// Set remaining variables
			curEmission = 0;
			bOKtoGetNextChunk = true;
			bFirstTimeThrough = false;
		}
	}

	
	
	// Get more XML if we are not already in the process of requesting XML
	//   and if it's time to get another chunk
	if (!bRequestingXML && curEmission >= (curTotalEmissions/2)) {
		if (bOKtoGetNextChunk) {
			bOKtoGetNextChunk = false;
			getNextXMLchunk();
		}
	}


	
	// Is it time to send the next OSC message?
	if ((ofGetElapsedTimeMillis() - lastEmitTimeMillis > nextEmitInterval) && (curEmission != curTotalEmissions)) {

		// Set initial variables
		curEmitID = -1;
		string data = "";
		
		
		// Read in and parse XML
		if (sResultsQueue.size() > 0){
			ofxXmlSettings curChunk;
			string sCurChunk;
			sCurChunk = sResultsQueue.front();
			curChunk.loadFromBuffer(sCurChunk);
			
			// Set emission-specific variables
			curChunk.pushTag("results"); {
				curChunk.pushTag("emission",curEmission); {
					curEmitID = ofToInt(curChunk.getValue("emitID","-2"));
					nextEmitInterval = ofToInt(curChunk.getValue("milliseconds","0"));
					data = curChunk.getValue("timestamp","");

					// If we did not get a timestamp, don't set the data string so we know not to send OSC message
					if (data != "") {
						data != "&*!";
						data += curChunk.getValue("activeParticles","1");
					}
				} curChunk.popTag();
			} curChunk.popTag();
			curChunk.clear();
		}
		

		// SEND OSC MESSAGE only if timestamp exists for this emission
		if (data != "") {
			LOG = "emit ID: " + ofToString(curEmitID) + "\n";
			sendOSCMessage(data);
		}
		
		
		// Update variables
		lastEmitTimeMillis = ofGetElapsedTimeMillis();
		curEmission++;
	}
}


//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(0,0,0);
	ofDrawBitmapString(LOG, 10, 10);
}


//--------------------------------------------------------------
void testApp::getNextXMLchunk() {
	LOG += "getNextXMLchunk\n";
	
	ofxHttpForm form;
	form.action = postURL;
	form.method = OFX_HTTP_POST;
	form.addFormField("getData","1");
	form.addFormField("date",sDate);
	form.addFormField("curEmitID",ofToString(highestEmitID));
	xmlGetter.addForm(form);
	
	bRequestingXML = true;
	LOG += "sent form\n";
}


//--------------------------------------------------------------
void testApp::receivedNextXMLchunk(ofxHttpResponse &response) {
	LOG += "received chunk\n";

	// Read in XML chunk to perform error checking
	ofxXmlSettings nextChunk;
	nextChunk.loadFromBuffer(response.responseBody);

	
	// ERROR CHECKING
	// Only add to queue if there are emission tags and if these
	//   results are not duplicates (checked for by 'highestEmitID' variable)
	bool bAddToQueue = false;
	nextChunk.pushTag("results"); {

		int numTags = nextChunk.getNumTags("emission");
		if (numTags) {
			// Now that we know we have good results, check to make sure this chunk
			//   isn't a duplicate of what we already have (since we are multi-threading)
			//   by confirming that we have an old emit ID compared to what we just got.
			nextChunk.pushTag("emission",numTags - 1); {
				int tempHighestEmitID = ofToInt(nextChunk.getValue("emitID","-1"));
				if (tempHighestEmitID > highestEmitID) {
					// GREAT!  We can add this XML chunk to the responseQueue!
					bAddToQueue = true;
					highestEmitID = tempHighestEmitID;
				}
			} nextChunk.popTag();

		} else if (nextChunk.getNumTags("error")) {
			cout << "ERROR GETTING XML RESULTS: " << nextChunk.getValue("error","") << endl;

		} else {
			// NOTE THAT THERE IS NO MORE XML TO GET
			bNoMoreXML = true;
		}
	} nextChunk.popTag();

	if (bAddToQueue) {
		LOG += "added new chunk to queue\n";
		sResultsQueue.push_back(response.responseBody);
	} else {
		LOG += "didn't add new chunk to queue\n(either because of error or repeat XML chunk)\n";
	}
	

	bRequestingXML = false;
	LOG += "done receiving\n";
}


//--------------------------------------------------------------
void testApp::sendOSCMessage(string data) {
	LOG += "sendOSCMessage: " + messageAddress + ":: " + data + "\n";
	
	ofxOscMessage m;
	m.setAddress(messageAddress);
	m.addStringArg(data);
	
	OSCsender.sendMessage(m);
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


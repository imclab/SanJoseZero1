#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	httpUtils.start();
		
	ofAddListener(httpUtils.newResponseEvent,this,&testApp::geoResponse);
	
	lat = "40.735844095042395";
	lon = "-73.9904522895813";
	radius = "15";
	printString = "waiting!";
	bSearching = false;
	
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
			lat = settings.getValue("lat","40.735844095042395");
			lon = settings.getValue("lon","-73.9904522895813");
			radius = settings.getValue("radius","15");
		}settings.popTag();
	};
	
	cout << "setting up sender at "<<host<<","<<port<<endl;
	
	//setup OSC
	sender.setup(host, port);
}

//--------------------------------------------------------------
void testApp::doSearch(){
	ofxHttpForm form;
	form.action = "http://api.flickr.com/services/rest/?method=flickr.photos.search";
	form.method = OFX_HTTP_POST;
	form.addFormField("api_key", API_KEY);
	form.addFormField("lat", lat);
	form.addFormField("lon", lon);
	form.addFormField("radius", radius);
	form.addFormField("extras", "date_taken");
	form.addFormField("sort", "date-taken-desc");
	//form.addFormField("auth_token", "72157624269831681-ab173df1cc178670");
	//form.addFormField("api_sig", "c0b180796e67e529eaae3633de5ef9bf");
	cout << form.name<<endl;
	
	httpUtils.addForm(form);
	printString = "searching...";
	bSearching = true;
}

//--------------------------------------------------------------
void testApp::geoResponse(ofxHttpResponse & response){
	last = latest;
	
	lastSearch = ofGetElapsedTimeMillis();
	bSearching = false;
	
	ofxXmlSettings xmlResponse;
	xmlResponse.loadFromBuffer(response.responseBody);
	
	cout<< response.responseBody<< endl;
	
	xmlResponse.pushTag("rsp");
	xmlResponse.pushTag("photos");
	
	latest.id = xmlResponse.getAttribute("photo", "id", "");
	latest.setDate(xmlResponse.getAttribute("photo", "datetaken", ""));
	latest.setUrl( xmlResponse.getAttribute("photo", "farm", ""),xmlResponse.getAttribute("photo", "server", ""), xmlResponse.getAttribute("id", "datetaken", ""), xmlResponse.getAttribute("secret", "datetaken", ""));
	
	if (latest.id != last.id){
		cout<<"NEW PHOTO! "<<latest.url<<endl;
		ofxOscMessage m;
		m.setAddress("/pluginplay/flickr");
		m.addStringArg(latest.url);
		sender.sendMessage(m);
	}
	
	printString = "Got "+ofToString(xmlResponse.getNumTags("photo"))+" photos.\nThe latest id is "+latest.id+"\nwhich was taken at "+latest.dateString;
	
	xmlResponse.popTag();
	xmlResponse.popTag();
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetElapsedTimeMillis() - lastSearch > SEARCH_TIME && !bSearching){
		doSearch();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofDrawBitmapString(printString, 20, 20);
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


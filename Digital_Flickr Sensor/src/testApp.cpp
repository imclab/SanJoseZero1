#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	geoSearcher.start();
	tagSearcher.start();
		
	ofAddListener(geoSearcher.newResponseEvent,this,&testApp::geoResponse);
	ofAddListener(tagSearcher.newResponseEvent,this,&testApp::tagResponse);
	
	lat = "40.735844095042395";
	lon = "-73.9904522895813";
	radius = "15";
	printGeoString = "waiting!";
	printTagString = "waiting!";
	
	bGeoSearching = false;
	bTagSearching = false;
	bGeoNew = false;
	bTagNew = false;
	
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
			tags = settings.getValue("tags", "new york");
		}settings.popTag();
	};
	
	cout << "setting up sender at "<<host<<","<<port<<endl;
	
	//setup OSC
	sender.setup(host, port);
	
	doGeoSearch();
	doTagSearch();
}

//--------------------------------------------------------------
void testApp::doGeoSearch(){
	ofxHttpForm form;
	form.action = "http://api.flickr.com/services/rest/?method=flickr.photos.search";
	form.method = OFX_HTTP_POST;
	form.addFormField("api_key", API_KEY);
	form.addFormField("lat", lat);
	form.addFormField("lon", lon);
	form.addFormField("radius", radius);
	form.addFormField("extras", "date_taken");
	form.addFormField("sort", "date-taken-desc");
	
	cout << form.name<<endl;
	
	geoSearcher.addForm(form);
	printGeoString = "searching...";
	bGeoSearching = true;
}

//--------------------------------------------------------------
void testApp::doTagSearch(){
	ofxHttpForm form;
	form.action = "http://api.flickr.com/services/rest/?method=flickr.photos.search";
	form.method = OFX_HTTP_POST;
	form.addFormField("api_key", API_KEY);
	form.addFormField("tags", tags);
	//form.addFormField("extras", "date_taken");
	//form.addFormField("sort", "date-taken-desc");
	
	tagSearcher.addForm(form);
	printTagString = "searching tags "+tags;
	bTagSearching = true;
}

//--------------------------------------------------------------
void testApp::tagResponse(ofxHttpResponse & response){
	lastSearch = latestSearch;
	
	lastTagSearch = ofGetElapsedTimeMillis();
	bTagSearching = false;
	
	ofxXmlSettings xmlResponse;
	xmlResponse.loadFromBuffer(response.responseBody);
	
	cout<< response.responseBody<< endl;
	
	xmlResponse.pushTag("rsp");
	xmlResponse.pushTag("photos");
	
	if (xmlResponse.getNumTags("photo") <= 0) return;
	
	latestSearch.id = xmlResponse.getAttribute("photo", "id", "");
	//latestSearch.setDate(xmlResponse.getAttribute("photo", "datetaken", ""));
	latestSearch.setUrl( xmlResponse.getAttribute("photo", "farm", ""),xmlResponse.getAttribute("photo", "server", ""), xmlResponse.getAttribute("id", "datetaken", ""), xmlResponse.getAttribute("secret", "datetaken", ""));
	
	if (latestSearch.id != lastSearch.id){
		cout<<"NEW PHOTO! "<<latestSearch.url<<endl;
		ofxOscMessage m;
		m.setAddress("/pluginplay/flickr/tags");
		m.addStringArg(latestSearch.url);
		sender.sendMessage(m);
		bGeoNew = true;
	} else {
		bGeoNew = false;
	}
	
	printTagString = "Got "+ofToString(xmlResponse.getNumTags("photo"))+" photos.\nThe latest id is "+latestSearch.id+"\nwhich was taken at "+latestSearch.dateString;
	
	xmlResponse.popTag();
	xmlResponse.popTag();
}

//--------------------------------------------------------------
void testApp::geoResponse(ofxHttpResponse & response){
	lastGeo = latestGeo;
	
	lastGeoSearch = ofGetElapsedTimeMillis();
	bGeoSearching = false;
	
	ofxXmlSettings xmlResponse;
	xmlResponse.loadFromBuffer(response.responseBody);
	
	cout<< response.responseBody<< endl;
	
	xmlResponse.pushTag("rsp");
	xmlResponse.pushTag("photos");
	if (xmlResponse.getNumTags("photo")  <= 0) return;
	latestGeo.id = xmlResponse.getAttribute("photo", "id", "");
	latestGeo.setDate(xmlResponse.getAttribute("photo", "datetaken", ""));
	latestGeo.setUrl( xmlResponse.getAttribute("photo", "farm", ""),xmlResponse.getAttribute("photo", "server", ""), xmlResponse.getAttribute("id", "datetaken", ""), xmlResponse.getAttribute("secret", "datetaken", ""));
	
	if (latestGeo.id != lastGeo.id){
		cout<<"NEW PHOTO! "<<latestGeo.url<<endl;
		ofxOscMessage m;
		m.setAddress("/pluginplay/flickr");
		m.addStringArg(latestGeo.url);
		sender.sendMessage(m);
		bTagNew = true;
	} else {
		bTagNew = false;
	}
	
	printGeoString = "Got "+ofToString(xmlResponse.getNumTags("photo"))+" photos.\nThe latest id is "+latestGeo.id+"\nwhich was taken at "+latestGeo.dateString;
	
	xmlResponse.popTag();
	xmlResponse.popTag();
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetElapsedTimeMillis() - lastGeoSearch > SEARCH_TIME && !bGeoSearching){
		doGeoSearch();
	}
	if (ofGetElapsedTimeMillis() - lastTagSearch > SEARCH_TIME && !bTagSearching){
		doTagSearch();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (bGeoNew){
		ofSetColor(255,0,0);
	} else {
		ofSetColor(255,255,255);
	}
	ofDrawBitmapString(printGeoString, 20, 20);
	
	if (bTagNew){
		ofSetColor(255,0,0);
	} else {
		ofSetColor(255,255,255);
	}
	ofDrawBitmapString(printTagString, 20, 100);
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
	ofxOscMessage m;
	m.setAddress("/pluginplay/flickr");
	m.addStringArg(latestSearch.url);
	sender.sendMessage(m);
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


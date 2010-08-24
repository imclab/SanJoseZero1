#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	std::string userName = "pluginplay";
	std::string password = "Kanarick";
	
	ofxXmlSettings settings;
	bool bLoaded = settings.loadFile("settings.xml");
	string host = "localhost";
	int port = 12345;
	if (bLoaded){
		settings.pushTag("settings");{
			host = settings.getValue("host","localhost");
			port = settings.getValue("port",12345);
			searchTerm = settings.getValue("search","play");
		}settings.popTag();
	};
	//setup OSC
	sender.setup(host, port);
	
	twitter.setTwitterUsername(userName);
	twitter.setTwitterPassword(password);
	lastSearch = 0;
	bNewEntry = false;
	latestSearchEntry = TwitterEntry();
	doSearch();
}


//--------------------------------------------------------------
void testApp::doSearch(){
	twitter.search(searchTerm);
	string response;
	twitter.getLastWebResponse(response);
	responseXML.loadFromBuffer(response);
	
	TwitterEntry oldEntry = latestSearchEntry;
	
	entries.clear();
	
	responseXML.pushTag("feed");{
		
		//get latest entry
		responseXML.pushTag("entry", 0);{
			
			latestSearchEntry.id = responseXML.getValue("id","");
			latestSearchEntry.published = responseXML.getValue("published","");
			latestSearchEntry.entry = responseXML.getValue("title","");
			
			responseXML.pushTag("author");{
				latestSearchEntry.author.name = responseXML.getValue("name","");
				latestSearchEntry.author.url = responseXML.getValue("uri","");
			} responseXML.popTag();
		} responseXML.popTag();
		
		//get all entries
		for (int i=0; i<responseXML.getNumTags("entry"); i++){
			responseXML.pushTag("entry", i);{
				
				TwitterEntry * newEntry = new TwitterEntry();
				
				newEntry->id = responseXML.getValue("id","");
				newEntry->published = responseXML.getValue("published","");
				newEntry->entry = responseXML.getValue("title","");
				
				responseXML.pushTag("author");{
					newEntry->author.name = responseXML.getValue("name","");
					newEntry->author.url = responseXML.getValue("uri","");
				} responseXML.popTag();
				
				entries.push_back(newEntry);
			} responseXML.popTag();
		}		
	} responseXML.popTag();
	
	if (oldEntry.id != latestSearchEntry.id){
		bNewEntry = true;
		ofxOscMessage m;
		m.setAddress("/pluginplay/twitter");
		m.addStringArg(latestSearchEntry.author.name);
		m.addStringArg(latestSearchEntry.entry);
		sender.sendMessage(m);
	} else {
		bNewEntry = false;
	}
	cout<<"here!"<<endl;
	
	lastSearch = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void testApp::update(){	
	if (ofGetElapsedTimeMillis() - lastSearch > SEARCH_TIME){
		doSearch();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (bNewEntry){
		ofBackground(0,150,150);
	} else {
		ofBackground(0,0,0);
	}
	cout<<"here?"<<endl;
	ofDrawBitmapString("newest tweet from: "+latestSearchEntry.author.name+"\n"+latestSearchEntry.entry+"\nat "+latestSearchEntry.published, 20, 30);
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


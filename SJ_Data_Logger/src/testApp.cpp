#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	//default values
	postUrl = "http://www.plug-in-play.com/php/storebuilding.php";
	
	int oscPort = 12005;
	
	ofxXmlSettings settings;
	settings.loadFile("settings.xml");
	settings.pushTag("settings");{
		oscPort = settings.getValue("osc:port", 12005);
		postUrl	= settings.getValue("server:url", postUrl);
	} settings.popTag();
	
	//setup osc
	receiver.setup(oscPort);
	
	//setup httputils
	ofAddListener(httpUtils.newResponseEvent,this,&testApp::newResponse);
}

//--------------------------------------------------------------
void testApp::update(){
	
	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		//unpack and send
		if (m.getAddress() == "/pluginplay/newrow"){
						
			ofxHttpForm form;
			form.action = postUrl;
			form.method = OFX_HTTP_POST;
			int index = 0;
			
			for (int i=0; i<m.getNumArgs(); i+=4){
				form.addFormField("buildings["+ofToString(index)+"][type]", m.getArgAsString(i));
				form.addFormField("buildings["+ofToString(index)+"][data]", m.getArgAsString(i+1));//b->getData());
				form.addFormField("buildings["+ofToString(index)+"][row]", m.getArgAsString(i+2));
				form.addFormField("buildings["+ofToString(index)+"][index]", m.getArgAsString(i+3));
				index++;
			};
			
			httpUtils.addForm(form);
		};
	}
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::newResponse(ofxHttpResponse & response){
	responseString = ofToString(response.status) + ": " + response.responseBody;
	cout <<responseString<<endl;
}
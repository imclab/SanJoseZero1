/*
    ofxHttpUtils v0.3
    Chris O'Shea, Arturo, Jesus, CJ

    Modified: 16th March 2009
    openFrameworks 0.06

*/


#include "ofxHttpUtils.h"
#include "ofEvents.h"

// ----------------------------------------------------------------------
ofxHttpUtils::ofxHttpUtils(){
    timeoutSeconds = 5;
    verbose = true;
    thereIsFormToSend = false;

    //start();
}
// ----------------------------------------------------------------------
ofxHttpUtils::~ofxHttpUtils(){
	//threadStartedMutex.lock();
    stop();
    //threadStartedMutex.unlock();
}
// ----------------------------------------------------------------------
void ofxHttpUtils::submitForm(ofxHttpForm form){
	doPostForm(form);
}

bool ofxHttpUtils::isThreadRunning(){
	FastMutex::ScopedLock lock(threadStartedMutex);
	return threadRunning;
}
// ----------------------------------------------------------------------
void ofxHttpUtils::addForm(ofxHttpForm form){

    printf("http: add form\n");
	lock();

    //forms.push(form);

    formToSend = form;
    thereIsFormToSend = true;

    unlock();
    printf("http: isThreadRunning = %i\n", (int)isThreadRunning());
    if (isThreadRunning() == false){
    	threadStartedMutex.lock();
        startThread(true, false);
        threadStartedMutex.unlock();
    }
}

// ----------------------------------------------------------------------
void ofxHttpUtils::start() {
     if (isThreadRunning() == false){
    	threadStartedMutex.lock();
    	startThread(true, true);
        threadStartedMutex.unlock();
        printf("thread started\n");
    }
}
// ----------------------------------------------------------------------
void ofxHttpUtils::stop() {
	threadStartedMutex.lock();
    stopThread();
    threadStartedMutex.unlock();
    printf("thread stopped\n");
}
// ----------------------------------------------------------------------
void ofxHttpUtils::threadedFunction(){

    // loop through this process whilst thread running
    while( isThreadRunning() == true ){
		lock();

		if(thereIsFormToSend){

			//ofxHttpForm form = forms.front();

			unlock();

			if(formToSend.method==OFX_HTTP_POST){
			    printf("ofxHttpUtils: (thread running) form submitted (post): %s\n", formToSend.name.c_str());
				doPostForm(formToSend);
			}else{
			    printf("ofxHttpUtils: (thread running) form submitted (get): %s\n", formToSend.name.c_str());
				string url = generateUrl(formToSend);
				getUrl(url);
			}
			thereIsFormToSend = false;
			lock();

			//forms.pop();
			unlock();
    	}else{

    		unlock();
            stop();
    	}
    }

}
// ----------------------------------------------------------------------
/*int ofxHttpUtils::getQueueLength(){
    // i have taken this out for now
     return 0;
     /*if( lock() ){
         printf("get queue size\n");
        return forms.size();
        unlock();
     }
}*/
// ----------------------------------------------------------------------
//void ofxHttpUtils::clearQueue(){
    // i have taken this out for now
//}
// ----------------------------------------------------------------------
string ofxHttpUtils::generateUrl(ofxHttpForm & form) {
    // url to send to
    string url = form.action;

    // do we have any form fields?
    /*
    int numfields = form.formIds.size();
    if(numfields > 0){
        url += "?";
        for(unsigned int i=0;i<numfields;i++){
            url += form.formIds[i] +"="+ form.formValues[i];
            if(i<numfields-1)
                url += "&";
        }
    }*/
    return url;
}

// ----------------------------------------------------------------------
void ofxHttpUtils::doPostForm(ofxHttpForm & form){

    try{
        URI uri( form.action.c_str() );
        std::string path(uri.getPathAndQuery());
        if (path.empty()) path = "/";

        HTTPClientSession session(uri.getHost(), uri.getPort());
        HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
        session.setTimeout( Poco::Timespan( timeoutSeconds, 0 ) );

        // create the form data to send
        HTMLForm pocoForm(HTMLForm::ENCODING_URL);

        // form values
        // this is taken out for now

        for(unsigned int i=0; i<form.formIds.size(); i++){
            const std::string name = form.formIds[i].c_str();
            const std::string val = form.formValues[i].c_str();
            pocoForm.set(name, val);
        }

        //pocoForm.set("name", "val");
        //pocoForm.set("name2", "val2");

        pocoForm.prepareSubmit(req);

        printf("------------------------------\n");
        printf("http: write the session\n");
        pocoForm.write(session.sendRequest(req));

        printf("http: about to receive a response\n");
        HTTPResponse res;
        istream& rs = session.receiveResponse(res);
        printf("http: received a session response\n");

        printf("http: create new response object\n");
		ofxHttpResponse response = ofxHttpResponse(res, rs, path);

        printf("http: broadcast response event\n");
    	ofNotifyEvent(newResponseEvent, response, this);

        printf("------------------------------\n\n");

        thereIsFormToSend = false;


    }catch (Exception& exc){

        thereIsFormToSend = false;

        printf("http: should catch error--\n");

        //ofNotifyEvent(notifyNewError, "time out", this);

        // for now print error, need to broadcast a response
        // std::cerr << exc.displayText() << std::endl;

    }

}

// ----------------------------------------------------------------------

void ofxHttpUtils::getUrl(string url){


   try{
		URI uri(url.c_str());
		std::string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		HTTPClientSession session(uri.getHost(), uri.getPort());
		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		session.setTimeout(Poco::Timespan(timeoutSeconds,0));
		session.sendRequest(req);
		HTTPResponse res;
		istream& rs = session.receiveResponse(res);

		ofxHttpResponse response=ofxHttpResponse(res, rs, path);

		ofNotifyEvent( newResponseEvent, response, this );

		//std::cout << res.getStatus() << " " << res.getReason() << std::endl;
		//StreamCopier::copyStream(rs, std::cout);

	}catch (Exception& exc){
		std::cerr << exc.displayText() << "\n";
	}


}
// ----------------------------------------------------------------------
void ofxHttpUtils::addUrl(string url){
	ofxHttpForm form;
	form.action=url;
	form.method=OFX_HTTP_GET;
    form.name=form.action;
	addForm(form);
}
// ----------------------------------------------------------------------



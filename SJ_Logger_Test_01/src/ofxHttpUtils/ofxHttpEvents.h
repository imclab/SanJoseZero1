/*
    ofxHttpUtils v0.3
    Chris O'Shea, Arturo, Jesus, CJ

    Modified: 16th March 2009
    openFrameworks 0.06

*/
#ifndef OFX_HTTP_EVENTS
#define OFX_HTTP_EVENTS


#include "ofxHttpUtils.h"

class ofxHttpEventListener{
public:
	virtual void newResponse( ofxHttpResponse & response ){};

};

#endif /* OFXHTTPEVENTS_H_ */

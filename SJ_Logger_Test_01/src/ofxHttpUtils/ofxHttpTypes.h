/*
    ofxHttpUtils v0.3
    Chris O'Shea, Arturo, Jesus, CJ

    Modified: 16th March 2009
    openFrameworks 0.06

*/
#ifndef OFX_HTTP_TYPES
#define OFX_HTTP_TYPES


#define OFX_HTTP_GET  0
#define OFX_HTTP_POST 1

struct ofxHttpForm{

	vector <string> formIds;
	vector <string> formValues;

    ofxHttpForm(){
    }
    ~ofxHttpForm(){
        clearFormFields();
    }

	// ----------------------------------------------------------------------
	void addFormField(string id, string value){
        formIds.push_back( id );
        formValues.push_back( value );
	}
	// ----------------------------------------------------------------------
	void clearFormFields(){
	    formIds.clear();
        formValues.clear();
	}
	int method;
	string action;
	string name;
};

#endif

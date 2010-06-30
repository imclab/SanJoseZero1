#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxHttpUtils.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"

/*
 Our API stuff
 key - 9d2a1eeb72830f58dd77d56672a3e54b 
 secret - 076ac459ed1b2cc9  
*/

#define API_KEY "9d2a1eeb72830f58dd77d56672a3e54b"
#define SECRET "076ac459ed1b2cc9"

#define SEARCH_TIME 5000

class FlickrPhoto
{
public:
	
	string id;
	string dateString;
	string url;
	
	int dateDay, dateMonth, dateYear;
	int dateTimeHours, dateTimeMinues, dateTimeSeconds;
	
	void setDate( string date){
		dateString = date;
		int spacePos = (int) date.find(" ");
		string dateM = date.substr(0, spacePos);
		string time = date.substr(spacePos+1);
		
		dateYear = atoi(dateM.substr(0,4).c_str());
		dateMonth = atoi(dateM.substr(5,2).c_str());
		dateDay = atoi(dateM.substr(8).c_str());
		
		dateTimeHours = atoi(time.substr(0,2).c_str());
		dateTimeMinues = atoi(time.substr(3,2).c_str());
		dateTimeSeconds = atoi(time.substr(6).c_str());
		
		cout<<dateYear<<":"<<dateMonth<<":"<<dateDay<<endl;
		cout<<dateTimeHours<<":"<<dateTimeMinues<<":"<<dateTimeSeconds<<endl;
	}
	
	void setUrl( string farmId, string server, string id, string secret){
		url = "http://farm"+farmId+".static.flickr.com/"+server+"/"+id+"_"+secret+".jpg";
	}
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		
		void geoResponse(ofxHttpResponse & response);
		
		void doSearch();
	
		string lat, lon;
		string radius;
	
		bool bSearching;
	
		ofxHttpUtils httpUtils;
		int lastSearch;
		string printString;
		ofxOscSender sender;
		FlickrPhoto latest;
		FlickrPhoto last;
};

#endif

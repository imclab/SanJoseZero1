#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxFft.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#define MIC 0
#define NOISE 1
#define SINE 2
#define MP3 3

enum Trigger_Mode_t {TM_NONE=0, TM_SETTING, TM_MOVING, TM_NAMING};

struct Trigger {
	int lowBand;
	int highBand;
	float height;
	string name;
	
	// ZACK BOKA: for storing multiple OSC message address
	vector<string> addresses;
	
	bool hit;
	bool sent;
};

class testApp : public ofBaseApp{

	public:
	
		~testApp();
	
		void setup();
		void update();
		void draw();

		void loadSettings();
		void saveSettings();
	
		void drawTriggers();
	
		void checkTriggers();
	
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		// ofxFft related
		void plot(float* array, int length, float yScale, int xScale, float yOffset);
		void audioReceived(float* input, int bufferSize, int nChannels);
	
		ofSoundPlayer song;	
		string songToLoad;
	
		int bufferSize;
		ofxFft* fft;
		float* audioInput;
		float* fftOutput;
		float* eqFunction;
		float* eqOutput;
		float* ifftOutput;
		
		// ofxFft related app values
		int mode;
		bool useEQ;
		
		// OSC related
		int lastFrame;
		string host;
		int port;
		ofxOscSender sender;
		void handleOSC();
		void sendSpectrum();
		void sendTriggers();
		bool sendFullSpectrum;
	
		vector <Trigger> triggers;
	
	//trigger stuff for drawing.
	ofPoint oldMouse;
	bool settingTrigger;
	Trigger_Mode_t triggerMode;
	
	int selTrigger;
};

#endif

#ifndef _TEST_APP
#define _TEST_APP
#include "ofxDirList.h"

#include "ofMain.h"

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
			
		ofxDirList settings;
		ofImage image1, image2;
		int lastLoaded;
};

#endif

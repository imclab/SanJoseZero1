#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "particle.h"

struct vort {
	int dir;
	particle v;
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

	vector<particle> particles;
//		vector <particle *> objects;
		vector <vort> vortexes;
		
		ofImage textures[10];
};

#endif

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxFBOTexture.h"
#include "ofxShader.h"
#include "ofx3DUtils.h"
#include "LaserUtils.h"
#include "LaserCamera.h"

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
	void setOrthographicProjection(float xMin, float xMax,
								   float yMin, float yMax, 
								   float zMin, float zMax);	
	void grabMatrix(GLenum gl_textureTarget);
	void drawTex(int x, int y, int w, int h);
	void gaussBlurKernel(int diameter,
						 float *_gVals,
						 float sigma,
						 float _stepX,
						 float _stepY);
	void setTextureMatrix(void);
	void drawRandomGeometry(bool grabM);
	void drawABox(float x, float y, float z, float size, bool grabM);
	
	//shaders
	ofxShader depthShader;
	ofxShader simpleTex;
	
	//fbo
	ofxFBOTexture fbo;
	
	LaserCamera camera;	

};

#endif

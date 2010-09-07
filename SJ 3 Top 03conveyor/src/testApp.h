#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofx3DUtils.h"
#include "ofxOsc.h"
#include "Emitter.h"

#include "ofxFBOTexture.h"
#include "ofxShader.h"
#include "LCurve.h"

#include "ofxLabProjectionTools.h"

#define PORT 12000

struct vert{
	ofxVec3f pos;
	ofxVec3f norm;
	float u,v;
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
	
	//calibration
		ofxLabProjectionTools projection;
		bool bDrawCurves;
	
	//LIGHTING
		ofxVec3f lightPosition;
		bool bLightSourceChanged;
	
	//LARS FUNCTIONS
	
		void grabMatrices(GLenum gl_textureTarget); 
		ofxVec3f normCalc( ofxVec3f a, ofxVec3f b, ofxVec3f c);
		void calcMeshNormals();
		void drawConveyorMesh();
		
		//ADJUST CONVEYOR
		void moveConveyorY(float dist);
		void scaleConveyor(int curveIndex, ofxVec3f scaleVal);
		void moveCurveX(int crvIndex, float xPos);
		void setupConveyorMesh();
		void scaleConveyorY(float scale);
		int getVertPointer(int curveIndex, float uPos);

		Emitter * particleManager;
		
		ofxLight light1; //this will be a directional light
		ofxLight light2; //this one a spot light
		ofxLight light3; //and this one a point light
		
		//logging stuff
		void rowIsComplete( BuildingRow * &completedRow);
	
	private:
		ofxOscReceiver	receiver;
		ofxOscSender	loggerSender;
		ofxOscSender	calibrationSender;
		bool bDragging;
	
		int numRows;
	
		//shaders
		ofxShader depthShader;
		ofxShader shadowShader;
		ofxShader simpleTex;
		
		//fbo
		ofxFBOTexture fbo;
		GLuint depthTexture;
		
		//misc
		int fCounter;
		
		//ceiling mesh
		vector <vert> vertices;
		vector <int> faceIndices;
		vector <ofxVec3f> faceNorms;
		vector <LCurve> curves;
		vector <pointOnCurveNode> meshNodes;
		vector <ofPoint> emitPoints;
		vector <ofxVec3f> refPoses;	//used for creating each curve
		int numSubdivisions;
		int numCurves;
		int numCVs;	
		float uIncrement;
		ofImage ceilingImage;
		
		//lars biuldings
		vector <Stack> stacks;
		vector <int> refVerts;
	
};

#endif

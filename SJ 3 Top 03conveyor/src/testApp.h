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

#define NUM_DRAW_MODES 6

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
	
	//calibration
		int lastEmitted;
		string ceilingImagePath;
	
		int drawMode;
		bool bUseProjectionTools;
		ofxLabProjectionTools projection;
		bool bDrawCurves;
		bool bInited;
		void setupGui();
		void updateFromGui();
	
		bool bWindowResized;
		float conveyorY;
		float rowRealBuffer;
	
	//  settings
		void loadSettings();
		bool bSaveSettings;
		void saveSettings();
	
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
		
		
		bool advanceCeiling;
		bool wireFrame;
		ofImage randImage;
		
		vector <pointOnCurveNode> sweetSpot;
		vector <ofxVec3f> sweetSpotPos;
		
		bool isCeilingAdvenced;
		
		ofxVec3f conveyorScale;
		float conveyorYoffset;
		float ceiling;
		
		//cached matrices
		
		double shadowModelView[16];
		float shadowModelViewFloat[16];
		double shadowProjection[16];
	
	private:
		//band-aid vars
		int fullScreenWaitTime, fullScreenStarted;
	
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
	
		//screen vars	
		float halfFov, theTan, screenFov, aspect;
	
		float eyeX;
		float eyeY;
		float dist;
		float nearDist; //dist / 50.0;	// near / far clip plane
		float farDist;
		ofxVec3f targetPos;
	
};

#endif

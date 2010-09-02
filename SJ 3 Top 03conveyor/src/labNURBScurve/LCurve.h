/*
 *  LCurve.h
 *  nurbsCurve
 *
 *  Created by lars berg on 9/28/2010.
 
 
 the CoxDeBoor calculation and point on curve calculation
 was taken/adapted from Rob Bateman's example code below:
 http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/
 *
 */

#ifndef _L_CURVE
#define _L_CURVE



#include "ofMain.h"
#include "ofxVectorMath.h"


class LCurve{
	
public:
	typedef void ( * _GLUfuncptr)();

	//constructors
	LCurve(){
		degree = 2;
		order = degree + 1;
		numSpans = 100;
		numKnots = 7 + order;
		knots = new float[numKnots];
		
		gluRes = 10.0;
	};
	~LCurve(){};
	
	//methods
	//void setup(int _numCVs,ofxVec3f _cv[], int _degree, int _numSpans);
	void setup(int _numCVs,ofxVec3f *_cv, int _degree, int _numSpans);
	//float CoxDeBoor(float u,int knot,int order,const float* Knots);
	ofxVec3f pointOnCurve(float u);
	void drawPoints();
	//void drawCurve();
	//void drawWithGluNurbs();
	void getPoints();
	void setGLuResolution(float _gluRes);
	void drawBorderPoly();
	void drawCVs();
	
	int getNumberOfCVs(){	return numCVs;}
	ofxVec3f getCVposition(int index){
		return ofxVec3f(controlVerts[index][0], controlVerts[index][1], controlVerts[index][2]);
	}
	//void moveCV(int index, ofxVec3f newPos);
	
	void testSetup(float _cv[][3]);
	bool ON_EvaluateNurbsDeBoor(int cv_dim, int order, int cv_stride, double *cv,
								const double *knots, int side, double mult_k, 
								double t);
	
	void CALLBACK vertexCallback(GLfloat *vertex);
	
	//new
	int getCVcount(){return (CVs.size()-1)/3;}
	void addCV(float x, float y, float z){
		CVs.push_back(x);
		CVs.push_back(y);
		CVs.push_back(z);
		numCVs = getCVcount();	//++;
	}
	ofxVec3f getCVpos(int index){
		index = MAX(0, MIN(index, getNumberOfCVs()-1));
		return ofxVec3f(CVs[index*3],
						CVs[index*3+1],
						CVs[index*3+2]);
	}
	void moveCV(int index, ofxVec3f newPos){
		index *= 3;
		CVs[index] = newPos.x;
		CVs[index+1] = newPos.y;
		CVs[index+2] = newPos.z;
	}		
	void moveCVrelative(int index, ofxVec3f newPos){
		if(index < getNumberOfCVs()-1){
			index *= 3;
			CVs[index] += newPos.x;
			CVs[index+1] += newPos.y;
			CVs[index+2] += newPos.z;
		}
	}		
	
	void setDegree(int newDegree){
		degree = MIN(7, MAX(1, newDegree));//above seven gets quite heavy
		order = degree + 1;
	}
	void setKnots(){		
		numKnots = numCVs + order;
		testKnots.clear();
		testKnots.reserve(numKnots);
		testKnots.resize(numKnots);
		for(int i=0; i<numKnots; i++){		
			/*our order is n, therefore we require n knots at each end
			 to clamp the curve to it's endpoints. For example,
			 with 6 points with an order of 4:		
			 0, 0, 0, 0, 0.333333, 0.666667, 1, 1, 1, 1
			 */		
			//this clamps the values between 0.0 and 1.0
			testKnots[i] = fmin(1.0, fmax(0.0, float(i-degree)/float(numCVs-degree)));
			//testKnots.push_back(fmin(1.0, fmax(0.0, float(i-order)/float(numCVs-order))));
			//testKnots.push_back(fmin(1.0, fmax(0.0, float(i-degree)/float(numCVs-degree))));
		}
	}
	
	void setNumSpans(int newSpanCount){numSpans = newSpanCount;}
	
	ofxVec3f pOnCurve(float u){	
		u = fmin(1.0, fmax(0.0, u));
		ofxVec3f pOnC;
		for(int i=0;i<numCVs;i++) {
			float val = CoxDeBoor(u,i,order);		
			if(val>0.001f) {
				pOnC += val * getCVpos(i);
			}
		}
		return pOnC;
	}
	
	float CoxDeBoor(float u,int knot, int order){
		if(order==1){
			if( testKnots[knot] <= u && u <= testKnots[knot+1] )return 1.0f;
			return 0.0f;
		}
		u = fmin(1.0, fmax(0.0, u));
		float Den1 = testKnots[knot+order-1] - testKnots[knot];
		float Den2 = testKnots[knot+order] - testKnots[knot+1];
		float Eq1=0,Eq2=0;
		
		if(Den1>0) {
			Eq1 = ((u-testKnots[knot])/Den1) * CoxDeBoor(u,knot,order-1); //recursive
		}
		if(Den2>0) {
			Eq2 = (testKnots[knot+order]-u)/Den2 * CoxDeBoor(u,knot+1,order-1); //recursive
		}
		return fmax(0.000001, fmin(0.999999, Eq1+Eq2)); //Eq1+Eq2;// 
		
	}
	
	
	void drawWithGluNurbs(){
		//not working correctly. I think its a problem with the knots.(works when using degrees and not order in definition)
		GLUnurbsObj* nurbs = gluNewNurbsRenderer();	
		gluNurbsProperty( nurbs,GLU_SAMPLING_TOLERANCE, gluRes);
		gluNurbsProperty(nurbs,GLU_NURBS_MODE,GLU_NURBS_RENDERER);
		
		gluBeginCurve(nurbs);
		gluNurbsCurve(nurbs,
					  (GLint)numKnots,
					  &testKnots[0],	//knots, 
					  3,
					  &CVs[0],	//controlVerts[0], 
					  (GLint)order,
					  GL_MAP1_VERTEX_3);
		gluEndCurve(nurbs);
		
		gluDeleteNurbsRenderer(nurbs);
	}
	void drawCurve(){
		glBegin(GL_LINE_STRIP);
		for(int i=0;i<numSpans;++i) {		
			float t  =  float(i) / float(numSpans-1);	
			ofxVec3f Outpoint = pOnCurve(t);
			glVertex3f(Outpoint.x, Outpoint.y, Outpoint.z);
		}
		glEnd();
	}
	
	//variables
	int numCVs;	
	ofxVec3f *cv;
	
	GLfloat (*controlVerts)[3];
	vector <GLfloat> CVs;
	int degree; //must be less then numCVs, it gets pretty heavy above 7 
	int order;
	float *knots;
	vector <GLfloat> testKnots;
	int numKnots;
	
	int numSpans;
	float gluRes;
	bool verbose;
	
	
};


class pointOnCurveNode{
public:
	//constructors
	pointOnCurveNode(){
		uPos = 0.5;
		usePositionPointer = false;
		findNormal = false;
	};	
	pointOnCurveNode(float _uPos, LCurve *_curve, ofxVec3f * _target){
		uPos = _uPos;
		curve = _curve;
		target = _target;
		usePositionPointer = true;
		findNormal = false;
		
		update();
	};
	
	void setup(float _uPos, LCurve *_curve, ofxVec3f * _target){
		uPos = _uPos;
		curve = _curve;
		target = _target;
		usePositionPointer = true;
		findNormal = false;
		
		update();
	};	
	
	void setup(float _uPos, LCurve *_curve){
		usePositionPointer = false;
		findNormal = true;//false;
		uPos = _uPos;
		curve = _curve;
		
		update();
	};	
	
	void setFindNormal(bool val){		
		findNormal = val;
	}
	
	void update(){
		pos = curve->pOnCurve(uPos);
		if(usePositionPointer)	*target = pos;
		if(findNormal) setRotation();
	}
	
	void setNorm(ofxVec3f newNormal){norm = newNormal;}
	void setRotation(){
		ofxVec3f upVec(0,0,1);
		ofxVec3f dir = curve->pOnCurve(uPos + 0.05);
		dir -= pos;
		
		angle = upVec.angle(dir)-90;
		//if(angle > 0.01){
			upVec.cross(dir);
		//}
	}
	
	//variables	
	float uPos;
	LCurve *curve;
	ofxVec3f * target;
	ofxVec3f norm;
	ofxVec3f pos;
	ofxVec3f rotAxis;
	float angle;
	
	
	bool usePositionPointer;
	bool findNormal;
	

};

#endif

#pragma once

#include "ofMain.h"
#include "ofxVectorMath.h"

class phyParticle
{
    public:
        ofxVec3f pos;
        ofxVec3f vel;
        ofxVec3f frc;  
			
        phyParticle();
		virtual ~phyParticle(){};

        void resetForce();
		void addForce(float x, float y);
		void addForce(float x, float y, float z);
		void addRepulsionForce(float x, float y, float radius, float scale);
		void addAttractionForce(float x, float y, float radius, float scale);
		
		void addRepulsionForce(phyParticle &p, float radius, float scale);
		void addAttractionForce(phyParticle &p, float radius, float scale);
		void addClockwiseForce(phyParticle &p, float radius, float scale);
		void addCounterClockwiseForce(phyParticle &p, float radius, float scale);
	
		
		void addDampingForce();
        
		void setInitialCondition(float px, float py, float pz, float vx, float vy, float vz=0);
        void update();
        void draw();
	
		void bounceOffWalls();
		
		ofColor color;
		float mass;
	
		float damping;
		float rot;
		bool bAlive;
		
	vector<ofPoint>trail;
	
	float rotChange;
	
	int type;
	
	ofImage * textures;

    protected:
    private:
};
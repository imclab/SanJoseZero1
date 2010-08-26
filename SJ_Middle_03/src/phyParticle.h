#pragma once

#include "ofMain.h"
#include "ofxVectorMath.h"

class phyParticle
{
    public:
        ofxVec2f pos;
        ofxVec2f vel;
        ofxVec2f frc;  
			
        phyParticle();
		virtual ~phyParticle(){};

        void resetForce();
		void addForce(float x, float y);
		void addRepulsionForce(float x, float y, float radius, float scale);
		void addAttractionForce(float x, float y, float radius, float scale);
		
		void addRepulsionForce(phyParticle &p, float radius, float scale);
		void addAttractionForce(phyParticle &p, float radius, float scale);
		void addClockwiseForce(phyParticle &p, float radius, float scale);
		void addCounterClockwiseForce(phyParticle &p, float radius, float scale);
	
		
		void addDampingForce();
        
		void setInitialCondition(float px, float py, float vx, float vy);
        void update();
        void draw();
	
		void bounceOffWalls();
	
	
		float damping;
		float rot;
	
	int mass;
	
	vector<ofPoint>trail;
	
	float rotChange;
	
	int type;
	
	ofImage * textures;

    protected:
    private:
};
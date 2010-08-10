#ifndef PARTICLE_H
#define PARTICLE_H

#include "ofMain.h"
#include "ofxVectorMath.h"

class particle
{
    public:
		string name;
        ofxVec2f pos;
        ofxVec2f vel;
        ofxVec2f frc;   // frc is also know as acceleration (newton says "f=ma")
			
        particle();
		virtual ~particle(){};

        void resetForce();
		void addForce(float x, float y);
		void addRepulsionForce(float x, float y, float radius, float scale);
		void addAttractionForce(float x, float y, float radius, float scale);
		
		void addRepulsionForce(particle &p, float radius, float scale);
		void addAttractionForce(particle &p, float radius, float scale);
		void addClockwiseForce(particle &p, float radius, float scale);
		void addCounterClockwiseForce(particle &p, float radius, float scale);
	
		
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
		ofImage * texture;

    protected:
    private:
};

#endif // PARTICLE_H
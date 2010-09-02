/*
 *  Particle.h
 *  SimpleSjCatcher
 *
 *  Created by brenfer on 6/30/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "BuildingType.h"
#include "ofx3DModelLoader.h"
#include "ofxVectorMath.h"

#define MAX_ROTATION_VEC_LENGTH 10

class Particle
{
public:
	
/***************************************************************
	CONSTRUCTOR
***************************************************************/
	
	ofColor color;
	bool bExploded;
	
	Particle(){
		bAlive = true;
		bSend = bSent = bExploded = false;
		
		setMinSpeed(5.);
		
		vel.x = 0;
		vel.y = -minSpeed;
		
		// Magnification
		bLeaving = false;
		bTransforming = false;
		
		frame = ofGetFrameNum();
		scale.x = scale.y = 2.0f;
		scale.z = .01;
		
		setTransformStart( ofGetHeight() * 3.0 / 4.0);
		setTransformEnd( ofGetHeight() / 4.0);
		
		extrudeTimer = 0;
		minScale = 2.0f;
		//maxScale = bMaxScale = 10.0f;
		setMaxScale( 10.0f);
		bDebugMode = false;
		bUpdated = false;
		bFoundNewColumn = false;
		drawType = -1;
		
		color.r = ofRandomuf()*255.;
		color.g = ofRandomuf()*255.;
		color.b = ofRandomuf()*255.;
		count = 0;
	}
	
	~Particle() {
		delete particle3D;
	}
	
/***************************************************************
	SET PROPERTIES + ASSETS
***************************************************************/
	
	// Associate this particle with a 3D model
	void setModel( ofx3DModelLoader* _particle3D){
		particle3D = _particle3D;
		particle3D->setScale(minScale, minScale, minScale);
		scale.x = scale.y = minScale;
	}
	
	void setScale( float _scale){
		scale = _scale;
	}
	
	//location
	
	void setLoc( float x, float y){
		pos.x = x;
		pos.y = y;
	}
	
	void setEndPoint( float x, float y, float z =0 ){
		endPoint.x = endPos.x = x;
		endPoint.y = y;
		endPoint.z = z;
	}
	
	void setEndPointX( float x ){
		endPoint.x = endPos.x = x;
	};
	
	ofxVec3f dampenVelocity ( float amt ){
		vel *= amt;
	}
	
	ofxVec3f getVelocity(){
		return vel;
	}
	
	ofxVec3f getLoc(){
		return pos;
	}
	
	ofxVec3f getRotatedLoc(){
		return rotatedPos;
	}
	
	ofxVec3f getEndPoint(){
		return endPoint;
	}
	
	void setMinScale( float mScale ){
		minScale = mScale;
	}
	
	void setMaxScale( float mScale ){
		if (bMaxScale == mScale) return;
		bMaxScale = maxScale = mScale;
		maxScale = mScale + ofRandomf()*5.;
		if (neighbors.size() > 8){
			maxScale /= 2.0f;
		} else if (neighbors.size() > 4){
			maxScale *= 1.5f;
		}
	}
	
	//frame
	
	int getFrame(){
		return frame;
	}
	
	//msg string
	
	void setMessageString( string msg ){
		messageString = msg;
	} 
	
	string getMessageString(){
		return messageString;
	}
	
	//data
	
	void setData( string d ){
		data = d;
	}
	
	string getData(){
		return data;
	}
	
	//color 
	void setColor(ofColor _color){
		color = _color;
	}
	
	void setColor ( float r, float g, float b ){
		color.r = r;
		color.g = g;
		color.b = b;
	}
	
	//dimensions
	
	float getHeight(){
		particle3D->setScale(1.0f, 1.0f, 1.0f);
		return particle3D->getYdim()*scale.y;
	}
	
	float getWidth(){
		particle3D->setScale(1.0f, 1.0f, 1.0f);
		return particle3D->getXdim()*scale.x;
	}
	
/***************************************************************
	 SET TARGET POINT(S)
***************************************************************/	
		
	ofPoint startPos, endPos;	
		
	void setTransformStart( float _startPos){
		startPos = _startPos;
		startPos.y += ofRandom(-50.f, 200.f);
	}
	
	void setTransformEnd( float _endPos ){
		endPos.y = _endPos;
	}
	
	void setTargetPoint( float x, float y, float z ){
		targetPoint.x = x;
		targetPoint.y = y;
		targetPoint.z = z;
	};
	
	void setTargetPointX( float x ){
		targetPoint.x = x;
	};
	
	void setTargetPointY( float y ){
		targetPoint.y = y;
	};
	
	
/***************************************************************
	 NEIGHBORS
***************************************************************/
	
	void clearNeighbors(){
		neighbors.clear();
	}
	
	void addNeighbor( Particle * p ){
		neighbors.push_back(p);
	}
	
	void setIndex( int _index ){
		index = _index;
	}
	
	bool isMasterParticle(){
		return index == 0 && neighbors.size() > 0;
	}
	
/***************************************************************
	 GET UPDATED POSITION OR VELOCITY
***************************************************************/
	
	//call getNextPoint(1) to get velocity
	//getNextPoint() or getNextPoint(0) return position
	
	// NOTE: does not update vars, just returns what next ones will be
	
	ofxVec3f getNextPoint(int which = 0){
		//localize variables to function
		ofxVec3f destPt, nPos, nAcc, nVel;
		nPos = pos;
		nAcc = acc;
		nVel = vel;
		
		// seek home particle if not particle (index=0)
		
		if ( (bTransforming || bLeaving) && index != 0){
			
			//steer
			float maxspeed = 30.0f; //arbitrary top speed for now...
			float maxforce = 2.0f;
			
			if (bLeaving || (neighbors[0]->bLeaving)){
				maxforce = 20.0f;
				if ( neighbors[index-1]->bUpdated){
					destPt = neighbors[index-1]->getLoc();
					destPt.y += neighbors[index-1]->getHeight()/2.0 + getHeight()/2.0;
				} else {
					destPt = neighbors[index-1]->getNextPoint();
					destPt.y += neighbors[index-1]->getHeight()/2.0 + getHeight()/2.0;
				};
				
			} else {			
				//destPt = neighbors[index-1]->getNextPoint();
				if ( neighbors[index-1]->bUpdated){
					destPt = neighbors[index-1]->getLoc();
					destPt.y += neighbors[index-1]->getHeight()/2.0 + getHeight()/2.0;
				} else {
					destPt = neighbors[index-1]->getNextPoint();
					destPt.y += neighbors[index-1]->getHeight()/2.0 + getHeight()/2.0;
				};
				
				destPt.x -= neighbors[index-1]->getWidth();
			}
			
			if (destPt.x < getWidth()) destPt.x = getWidth();
			else if (destPt.x > ofGetWidth()) destPt.x = ofGetWidth() - getWidth();
			
			ofxVec3f steer;  // The steering vector
			ofxVec3f desired;  // A vector pointing from the location to the target
			float d;			
			
			desired = destPt - nPos;  // A vector pointing from the location to the target
			d = ofDist3D(destPt.x, destPt.y, destPt.z, nPos.x, nPos.y, nPos.z);
			
			// If the distance is greater than 0, calc steering (otherwise return zero vector)
			if (d > 0) {				
				desired /= d; // Normalize desired
				desired *= maxspeed;
				if (d < 100) desired *= d/100.0f;
				// Steering = Desired minus Velocity
				steer = desired - nVel;
				steer.x = ofClamp(steer.x, -maxforce, maxforce); // Limit to maximum steering force
				steer.y = ofClamp(steer.y, -maxforce, maxforce);
				steer.z = ofClamp(steer.z, -maxforce, maxforce); 
			}
			
			nAcc += steer;
			
		//fly around all crazy if home particle + have children
			
		} else if ( (bTransforming || bLeaving) && index == 0){
			
			//steer
			float maxspeed = 30.0f; //arbitrary top speed for now...
			float maxforce = 2.0f;
			
			// force slowness unless on the way out
			if (bLeaving){
				maxspeed = minSpeed;
			}
			
			ofxVec3f steer;  // The steering vector
			ofxVec3f desired;  // A vector pointing from the location to the target
			float d;
			
			desired = targetPoint - nPos;  // A vector pointing from the location to the 
			d = ofDist3D(targetPoint.x, targetPoint.y, targetPoint.z, nPos.x, nPos.y, nPos.z);
			
			// If the distance is greater than 0, calc steering (otherwise return zero vector)
			if (d > 10) {				
				desired /= d; // Normalize desired
				desired *= maxspeed;
				
				//slow down unless on the way out
				if (d < 100 && !bLeaving){
					desired *= d/100.0f;
				} 
				// Steering = Desired minus Velocity
				steer = desired - nVel;
				steer.x = ofClamp(steer.x, -maxforce, maxforce); // Limit to maximum steering force
				steer.y = ofClamp(steer.y, -maxforce, maxforce); 
				steer.z = ofClamp(steer.z, -maxforce, maxforce); 
			} else if (!bLeaving && !bSend) {
				if (targetPoint.y == endPos.y){
					targetPoint.x = endPoint.x;
					targetPoint.y = endPoint.y;
					targetPoint.z = 0;
				} else {
					targetPoint.x = ofRandom(getWidth(),ofGetWidth()-getWidth());
					targetPoint.y = nPos.y - ofRandom(50,250);
					//targetPoint.z = ofRandom(-500,0);
					
					if (targetPoint.y <= endPos.y){
						targetPoint.x = endPoint.x;
						targetPoint.y = endPoint.y;
						targetPoint.z = 0;
					}
				}
			} /*else {
			   bAlive = false;
			   };*/
						
			nAcc += steer;
		}
		
		nVel += nAcc;
		//nPos += nVel;
		if ((!bLeaving || bSend) || index == 0){
			nPos += nVel;
		} else {
			nPos = destPt;
		}
		if (bTransforming){
			if (nPos.y >startPos.y){
				nPos.y = startPos.y;
				//nVel.y *= -ofRandom(1.,2.75f);
			}
		}
		//cheater variable
		if (which == 0){
			return nPos;
		} else {
			return nVel;
		}
	};

/***************************************************************
	UPDATE
***************************************************************/
	
	void update(){
		//store last value
		prevRotation = rotation;
		prevPos = pos;
		
		// Check to see if we are still alive
		if (pos.y <= 0 && !bSend){
			bSend = true;
		}
		
		if (!bAlive) return;
		
		if (pos.y <= endPoint.y){
			bAlive = false;
						
			return;
		} else if (index == 0){
			bool nAlive = false;
			
			for (int i=0; i<neighbors.size(); i++){
				if (neighbors[i]->bAlive){
					nAlive = true;
				};
			};
			if (!nAlive && neighbors.size() > 0){
				bAlive = false;
				return;
			}
		}	
		
		// are we past the threshold? + are we a follower particle?
		
		if (pos.y < startPos.y && !bTransforming && !bLeaving){
			bTransforming = true;
			
			targetPoint.x = pos.x + ofRandom(-200, 200);
			targetPoint.y = pos.y - ofRandom(50, 200);
		}		
		
	//alter scale and rotation
		
		//rotatiom
		if (bTransforming){
			if (index == 0){
				ofxVec3f heading = getNextPoint(1).getNormalized()*90.0f;
				rotation -= (rotation - heading)/5.0f;
			} else {
				rotation -= (rotation - neighbors[0]->rotation)/50.0f;
			}
		}
		
		//new values
		
		if (bTransforming){
			if (extrudeTimer < 10){
				extrudeTimer++;
				scale.z = ofLerp(.01, maxScale, extrudeTimer/10.);
				scale.x = ofLerp(minScale, maxScale, extrudeTimer/10.);
				scale.y = ofLerp(minScale, maxScale, extrudeTimer/10.);
				targetScale = maxScale;
			} else {
				scale.x -= (scale.x - targetScale)/10.f;
				scale.y -= (scale.y - targetScale)/10.f;
				scale.z -= (scale.z - targetScale)/10.f;
				//bTransforming = false;
				targetScale -= (targetScale - maxScale)/10.f;
			}
			
			if (rotation.x >= 360){
				rotation.x = rotation.y = rotation.z -= 360;
			} else if (rotation.x <= -360){
				rotation.x = rotation.y = rotation.z += 360;
			}
			
		//scaling back to normal
			
		} else if (bLeaving){
			if (extrudeTimer < 10){
				extrudeTimer++;
				scale.z = ofLerp(maxScale, .01, extrudeTimer/10.);
				scale.x = ofLerp(maxScale, minScale, extrudeTimer/10.);
				scale.y = ofLerp(maxScale, minScale, extrudeTimer/10.);
				
				rotation.x = ofLerp(lastRotation.x, targetRotation.x, extrudeTimer/10.);
				rotation.y = ofLerp(lastRotation.y, targetRotation.y, extrudeTimer/10.);
				rotation.z = ofLerp(lastRotation.z, targetRotation.z, extrudeTimer/10.);
				
				if (index == 0){
					pos.x = ofLerp(lastPosition.x, endPos.x, extrudeTimer/10.);
					pos.y = ofLerp(lastPosition.y, endPos.y, extrudeTimer/10.);
					pos.z = ofLerp(lastPosition.z, endPos.z, extrudeTimer/10.);
					 
				};
				
			} else {
				scale.x = scale.y = minScale;
				scale.z = .01;
				rotation.x = targetRotation.x;
				rotation.y = targetRotation.y;
				rotation.z = targetRotation.z;
				
				//if (bSend){
				if ( index == 0 ){
					if (extrudeTimer == 10){
						pos.x = endPos.x;
						pos.y = endPos.y;
						pos.z = endPos.z;
					}
					extrudeTimer++;
					/*
					pos.x = ofLerp(endPos.x, endPoint.x, (extrudeTimer - 10.0f)/100.0);
					pos.y = ofLerp(endPos.y, endPoint.y, (extrudeTimer - 10.0f)/100.0);
					pos.z = ofLerp(endPos.z, endPoint.z, (extrudeTimer - 10.0f)/100.0);
					 */
				};
			};
		};
		
		if (( pos.y <= endPos.y /*|| (index != 0 && neighbors[0]->bLeaving)*/ ) && !bLeaving){
			bTransforming = false;
			bLeaving = true;
			extrudeTimer = 0;
			
			lastRotation = rotation;
			
			//find the closest '0' rotation
			
			ofPoint rotateZero = 0.0f;
			ofPoint rotate360 = ofPoint(360,360,360);
			ofPoint rotate360N = ofPoint(-360,-360,-360);
						
			float d1 = lastRotation.distance(rotateZero);
			float d2 = lastRotation.distance(rotate360);
			float d3 = lastRotation.distance(rotate360N);
			
			float mininmum = fmin(d1, min(d2, d3));
			
			if (mininmum == d1){
				targetRotation = rotateZero;
			} else if (mininmum == d2){
				targetRotation = rotate360;
			} else {
				targetRotation = rotate360N;
			}
			lastPosition = pos;
		};
		
		//update position
		
		//vel += acc;
		//pos += vel;
				
		vel = getNextPoint(1);
		pos = getNextPoint();
		
		//if ((!bLeaving || bSend) || index == 0){
		/*} else {
			pos = neighbors[index-1]->getLoc();
			pos.y += neighbors[index-1]->getHeight();
		}*/
		acc = 0;	
		bUpdated = true;
	}
		
	float ofDist3D(float x1, float y1, float z1, float x2, float y2, float z2 ) {
		return sqrt(double((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2)));
	}

/***************************************************************
	DRAW
***************************************************************/

	
	void draw(){
		ofPushMatrix();{
			rotatedPos = pos;
			if (index != 0){
				rotatedPos.rotate(1.0, neighbors[0]->getLoc(), rotation);
				/*
				ofTranslate(neighbors[0]->getLoc().x, neighbors[0]->getLoc().y, neighbors[0]->getLoc().z);
				ofRotateX(rotation.x);
				ofRotateY(rotation.y);
				ofRotateZ(rotation.z);
				
				//ofRotateX(rotation.x);
				//ofRotateY(rotation.y);
				//ofRotateZ(rotation.z);
				ofTranslate(-neighbors[0]->getLoc().x, -neighbors[0]->getLoc().y, -neighbors[0]->getLoc().z);*/
			} 
			ofTranslate(rotatedPos.x, rotatedPos.y, rotatedPos.z);
			//ofTranslate(pos.x, pos.y, pos.z);
			//if (index == 0){
				ofRotateX( rotation.x );
				ofRotateY( rotation.y );
				ofRotateZ( rotation.z );
			//}
			
			if (bDebugMode){
				ofSetColor(index*150,50,50);
				ofRect(-getWidth()/2.0f,-getHeight()/2.0f, getWidth(), getHeight());
				ofSetColor(0xffffff);
				ofDrawBitmapString(ofToString(index), -getWidth(), 0);
				
			} else {
				particle3D->setScale(scale.x,scale.y,scale.z);
				particle3D->draw();
				ofSetColor(index*150,50,50);
			}
		} ofPopMatrix();
		bUpdated = false;
		
		//debug
	};
	
	double count;
	
	void drawTrail( float rotate=0.0f){
		ofPushMatrix();{
			rotatedPos = pos;
			if (index != 0){
				rotatedPos.rotate(1.0, neighbors[0]->getLoc(), rotation);
				/*
				 ofTranslate(neighbors[0]->getLoc().x, neighbors[0]->getLoc().y, neighbors[0]->getLoc().z);
				 ofRotateX(rotation.x);
				 ofRotateY(rotation.y);
				 ofRotateZ(rotation.z);
				 
				 //ofRotateX(rotation.x);
				 //ofRotateY(rotation.y);
				 //ofRotateZ(rotation.z);
				 ofTranslate(-neighbors[0]->getLoc().x, -neighbors[0]->getLoc().y, -neighbors[0]->getLoc().z);*/
			} 
			ofTranslate(rotatedPos.x, rotatedPos.y, rotatedPos.z);
			if (index == 0){
				ofRotateX( rotation.x );
				ofRotateY( rotation.y );
				ofRotateZ( rotation.z );
			}
			
			ofRotateZ(rotate);
						
			count +=.1;
						
			particle3D->setScale(scale.x/2.0f + sinf(count)*2.0f,scale.y/2.0f*2.0f+ sinf(count)*2.0f,scale.z/2.0f+ sinf(count)*2.0f);
			ofSetColor(color.r, color.g, color.b);
			particle3D->draw();
			
		} ofPopMatrix();
	};

	bool alive() {
		return bAlive;
	};
	
	bool bSent;
	
	bool send(){
		return bSend;
	}
	
	bool okToErase(){
		bool good = true;
		
		for (int i=0; i<neighbors.size(); i++){
			if (!neighbors[i]->bSent){
				good = false;
			};
		};
		
		if (good){
			if (index == 0){
				for (int i=0; i<neighbors.size(); i++){
					neighbors[i]->bAlive = false;
				}
			}
		}
		
		if (!good && neighbors.size() > 0){
			return false;
		} else if (good || neighbors.size() == 0){
			return true;
		}
		
	};
	
/***************************************************************
	 PUBLIC PROPS
***************************************************************/
	
	// three steps: emitted, transforming, leaving
	bool bTransforming;
	bool bLeaving;
	
	// switches for checking + updating position
	bool bUpdated;
	bool bFoundNewColumn;
	
	ofxVec3f rotation, lastRotation, targetRotation;
	int index;
	
	//base particle vars
	
	ofxVec3f pos, vel, acc, rotatedPos;
	ofxVec3f prevPos, prevRotation;
	
	//type
	
	BuildingType * getType(){
		return type;
	}
	
	BuildingType * setType( BuildingType * _t ){
		type = _t;
	}
	
	//speed
	
	void setMinSpeed( float speed ){
		minSpeed = speed;
		
		//if not doing anything yet, update base velocity
		if (!bTransforming && !bLeaving){
			vel.y = -minSpeed;
		}
	};
	
	int drawType;
	ofPoint scale;
	float targetScale;
	
private:
	bool bDebugMode;
	
	BuildingType * type;
	
	//important storage vars!
	string messageString;
	string data;
	
	//timing vars
	int duration, subDuration;
	
	bool bAlive,bSend;
	float minScale, maxScale, bMaxScale;
	float minSpeed;
	
	//NEIGHBOR VARS:
	int frame;
	vector<Particle *>neighbors;
	
	//extruding
	int extrudeTimer;
	
	// 3D and Rotation
	ofx3DModelLoader* particle3D;
	
	// points to target
	ofxVec3f endPoint;
	ofxVec3f targetPoint;
	ofPoint lastPosition;
};

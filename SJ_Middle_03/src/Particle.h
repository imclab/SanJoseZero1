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
#include "ofxTween.h"
#include "ofx3DModelLoader.h"
#include "ofxVectorMath.h"

#define MAX_ROTATION_VEC_LENGTH 10


class Particle
{
public:
	
/***************************************************************
	CONSTRUCTOR
***************************************************************/
	
	Particle(){
		bAlive = true;
		
		bIn3D = false;
		
		vel.x = 0;
		vel.y = -5;
		
		// Rotation
		bPre3D = true;
		
		float rotationDir = ofRandomuf();
		if (rotationDir < 0.5)
			rotationDirection = -1;
		else
			rotationDirection = 1;
		
		rotateSpeed = ofRandom(3.0f, 10.0f);

		// Magnification
		bOkToMagnify = true;
		bOkToDeMagnify = false;
		bMagnifying = false;
		
		frame = ofGetFrameNum();
		scale.x = scale.y = 2.0f;
		scale.z = .1;
		
		setTweenTime(3000);
		setTweenStart( ofGetHeight() * 3.0 / 4.0);
		setTweenEnd( ofGetHeight() / 4.0);
		
		extrudeTimer = 0;
		minScale = 2.0f;
		maxScale = 10.0f;
		bDebugMode = false;
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
	
	ofxVec3f getLoc(){
		return pos;
	}
	
	ofxVec3f getEndPoint(){
		return endPoint;
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
	 START TWEEN(S)
***************************************************************/	
	
	bool xTweenComplete, yTweenComplete, zTweenComplete;
	
	ofPoint startPos, endPos;	
	
	void setTweenTime( int tweenTime ){
		duration = tweenTime;
	}
	
	void setTweenStart( float _startPos){
		startPos = _startPos;
	}
	
	void setTweenEnd( float _endPos ){
		endPos.y = _endPos;
	}
	
/***************************************************************
	 NEIGHBORS
***************************************************************/
	
	void addNeighbor( Particle * p ){
		neighbors.push_back(p);
	}
	
	void setIndex( int _index ){
		index = _index;
	}

/***************************************************************
	UPDATE
***************************************************************/
	
	void update(){
		// Check to see if we are still alive
		if (pos.y + particle3D->getYdim()*2.0 < endPoint.y){
			bAlive = false;
			return;
		}
		
		ofxVec3f destPt;
		
		// are we past the threshold? + are we a follower particle?
		
		if (pos.y < startPos.y && !bMagnifying && !bOkToDeMagnify){
			bMagnifying = true;
			
			targetPoint.x = ofRandom(0, ofGetWidth());
			targetPoint.y = pos.y - 50;
		}
		
		// seek home particle if not particle #0
		
		if ( (bMagnifying || bOkToDeMagnify) && index != 0){
			
			//steer
			float maxspeed = 30.0f; //arbitrary top speed for now...
			float maxforce = 2.0f;
			
			ofxVec3f steer;  // The steering vector
			ofxVec3f desired;  // A vector pointing from the location to the target
			float d;
			
			destPt = neighbors[index-1]->getLoc();
			destPt.y += neighbors[index-1]->getHeight();
			desired = destPt - pos;  // A vector pointing from the location to the target
			d = ofDist3D(destPt.x, destPt.y, destPt.z, pos.x, pos.y, pos.z);
			
			// If the distance is greater than 0, calc steering (otherwise return zero vector)
			if (d > 0) {				
				desired /= d; // Normalize desired
				desired *= maxspeed;
				if (d < 100) desired *= d/100.0f;
				// Steering = Desired minus Velocity
				steer = desired - vel;
				steer.x = ofClamp(steer.x, -maxforce, maxforce); // Limit to maximum steering force
				steer.y = ofClamp(steer.y, -maxforce, maxforce); 
				steer.z = ofClamp(steer.z, -maxforce, maxforce); 
			}
			
			acc += steer;
			
		//fly around all crazy if home particle + have children
		} else if ( bMagnifying && !bOkToDeMagnify && neighbors.size() > 0){
			
			//steer
			float maxspeed = 30.0f; //arbitrary top speed for now...
			float maxforce = 2.0f;
			
			ofxVec3f steer;  // The steering vector
			ofxVec3f desired;  // A vector pointing from the location to the target
			float d;
			
			desired = targetPoint - pos;  // A vector pointing from the location to the 
			d = ofDist3D(targetPoint.x, targetPoint.y, targetPoint.z, pos.x, pos.y, pos.z);
				
			// If the distance is greater than 0, calc steering (otherwise return zero vector)
			if (d > 0.5) {				
				desired /= d; // Normalize desired
				desired *= maxspeed;
				if (d < 100){
					desired *= d/100.0f;
				} 
				// Steering = Desired minus Velocity
				steer = desired - vel;
				steer.x = ofClamp(steer.x, -maxforce, maxforce); // Limit to maximum steering force
				steer.y = ofClamp(steer.y, -maxforce, maxforce); 
				steer.z = ofClamp(steer.z, -maxforce, maxforce); 
			} else {
				targetPoint.x = ofRandom(0,ofGetWidth());
				targetPoint.y = pos.y - ofRandom(50,250);
				
				if (targetPoint.y < endPos.y){
					cout<<"targeting "<<endPoint.x<<":"<<endPoint.y<<endl;
					targetPoint.x = endPoint.x;
					targetPoint.y = endPoint.y;
				}
			}
			
			acc += steer;
		};
		
	//alter scale and rotation
		
		if (bMagnifying){
			if (extrudeTimer < 10){
				extrudeTimer++;
				scale.z = ofLerp(.1, maxScale, extrudeTimer/10.);
				scale.x = ofLerp(minScale, maxScale, extrudeTimer/10.);
				scale.y = ofLerp(minScale, maxScale, extrudeTimer/10.);
			} else {
				scale.x = maxScale;
				scale.y = maxScale;
				scale.z = maxScale;
				//bMagnifying = false;
			}
			
			rotation.x += rotateSpeed*rotationDirection;
			rotation.y += rotateSpeed*rotationDirection;
			rotation.z += rotateSpeed*rotationDirection;
			if (rotation.x >= 360){
				rotation.x = rotation.y = rotation.z -= 360;
			} else if (rotation.x <= -360){
				rotation.x = rotation.y = rotation.z += 360;
			}
			
		} else if (bOkToDeMagnify){
			if (extrudeTimer < 10){
				extrudeTimer++;
				scale.z = ofLerp(maxScale, .1, extrudeTimer/10.);
				scale.x = ofLerp(maxScale, minScale, extrudeTimer/10.);
				scale.y = ofLerp(maxScale, minScale, extrudeTimer/10.);
				
				rotation.x = ofLerp(lastRotation.x, 0.0f, extrudeTimer/10.);
				rotation.y = ofLerp(lastRotation.y, 0.0f, extrudeTimer/10.);
				rotation.z = ofLerp(lastRotation.z, 0.0f, extrudeTimer/10.);
				
				if (index == 0){
					pos.x = ofLerp(lastPosition.x, endPos.x, extrudeTimer/10.);
					pos.y = ofLerp(lastPosition.y, endPos.y, extrudeTimer/10.);
					pos.z = ofLerp(lastPosition.z, endPos.z, extrudeTimer/10.);
				};
				
			} else {
				scale.x = scale.y = minScale;
				scale.z = .1;
				rotation.x = targetRotation.x;
				rotation.y = targetRotation.y;
				rotation.z = targetRotation.z;
				
				if (index == 0 && extrudeTimer < 110.0f){
					extrudeTimer++;
					pos.x = ofLerp(endPos.x, endPoint.x, (extrudeTimer - 10.0f)/100.0f);
					pos.y = ofLerp(endPos.y, endPoint.y, (extrudeTimer - 10.0f)/100.0f);
					pos.z = ofLerp(endPos.z, endPoint.z, (extrudeTimer - 10.0f)/100.0f);
				}
			}
		};
		
		if (( pos.y < endPos.y || (index != 0 && neighbors[0]->bOkToDeMagnify) ) && !bOkToDeMagnify){
			bMagnifying = false;
			bOkToDeMagnify = true;
			extrudeTimer = 0;
			
			lastRotation = rotation;
			
			if (lastRotation.x < 0){
				targetRotation.x = -0.0f;
				targetRotation.y = -0.0f;
				targetRotation.z = -0.0f;
			} else {
				targetRotation.x = 360.0f;
				targetRotation.y = 360.0f;
				targetRotation.z = 360.0f;
			}			
			lastPosition = pos;
		}
		
		//update position
		
		vel += acc;
		pos += vel;
		
		acc = 0;	
	}
	
	
	float ofDist3D(float x1, float y1, float z1, float x2, float y2, float z2 ) {
		return sqrt(double((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2)));
	}

/***************************************************************
	DRAW
***************************************************************/

	
	void draw(){ 
		ofPushMatrix();{
			if (index != 0){
				ofTranslate(neighbors[0]->getLoc().x, neighbors[0]->getLoc().y, neighbors[0]->getLoc().z);
				ofRotateX(neighbors[0]->rotation.x);
				ofRotateY(neighbors[0]->rotation.y);
				ofRotateZ(neighbors[0]->rotation.z);
				
				//ofRotateX(rotation.x);
				//ofRotateY(rotation.y);
				//ofRotateZ(rotation.z);
				ofTranslate(-neighbors[0]->getLoc().x, -neighbors[0]->getLoc().y, -neighbors[0]->getLoc().z);
			} 
			
			ofTranslate(pos.x, pos.y, pos.z);
			if (index == 0){
				ofRotateX( rotation.x );
				ofRotateY( rotation.y );
				ofRotateZ( rotation.z );
			}
			// Adjust rotation
			//particle3D->setRotation(0,rotationCtr * rotationDirection,xRotateVec,yRotateVec,zRotateVec);
			if (bDebugMode){
				ofSetColor(index*150,50,50);
				ofRect(-getWidth()/2.0f,-getHeight()/2.0f, getWidth(), getHeight());
				ofSetColor(0xffffff);
				ofDrawBitmapString(ofToString(index), -getWidth(), 0);
			} else {
				particle3D->setScale(scale.x,scale.y,scale.z);
				particle3D->draw();
			}
		} ofPopMatrix();
	};
	

	bool alive() {
		return bAlive;
	};
	bool bMagnifying;
	bool bOkToMagnify;
	bool bOkToDeMagnify;
	
	ofPoint rotation, lastRotation, targetRotation;
	
private:
	bool bDebugMode;
	
	//position
	ofxVec3f pos, vel, acc;
	
	//important storage vars!
	string messageString;
	string data;
	
	//timing vars
	int duration, subDuration;
	
	bool bAlive;
	ofPoint scale;
	float minScale, maxScale;
	float rotateSpeed;
	
	//NEIGHBOR VARS:
	int frame;
	int index;
	vector<Particle *>neighbors;
	
	//extruding
	int extrudeTimer;
	
	// Magnification
	
	int rotationDirection;
	bool bPre3D;	
	
	// 3D and Rotation
	bool bIn3D;
	ofx3DModelLoader* particle3D;
	
	// points to target
	ofxVec3f endPoint;
	ofxVec3f targetPoint;
	ofPoint lastPosition;
};

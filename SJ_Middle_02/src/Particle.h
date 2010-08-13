/*
 *  Particle.h
 *  SimpleSjCatcher
 *
 *  Created by brenfer on 6/30/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once
#include "ofxTween.h"
#include "ofx3DModelLoader.h"

#define SPEED 3
#define MAX_ROTATION_VEC_LENGTH 10


class Particle
{
public:
	
	Particle(){
		bAlive = true;
		
		// 3D
		bIn3D = false;
		scale = 1.0f;

		
		// Rotation
		rotationCtr = 0.0;
		xRotateVec = ofRandom(0.0, MAX_ROTATION_VEC_LENGTH);
		yRotateVec = ofRandom(0.0, MAX_ROTATION_VEC_LENGTH);
		zRotateVec = ofRandom(0.0, MAX_ROTATION_VEC_LENGTH);
		float rotationDir = ofRandomuf();
		if (rotationDir < 0.5)
			rotationDirection = -1;
		else
			rotationDirection = 1;


		// Magnification
		prevScale = 0.0;
		bOkToMagnify = true;
		bOkToDeMagnify = false;
		bMagnifying = false;
		maxMagnificationSize = ofGetWidth() / (10 * 20);
	}
	
	
	~Particle() {
		delete particle3D;
	}
	
	
	// Associate this particle with a 3D model
	void setModel( ofx3DModelLoader* _particle3D){
		particle3D = _particle3D;
	}

	
	void setScale( float _scale){
		scale = _scale;
	}
	
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	
	void update(){
		// Check to see if we are still alive
		if (loc.y + ofGetHeight() / 8.0 <= 0) {
			bAlive = false;
			return;
		}
		

		
		float curScale;
		
		// First check if we are in the magnification or demagnification process, which adjusts the y-axis location, and does tweened rotation
		if (bMagnifying) {
			// Adjust scale
			curScale = particleTween.update();
			particle3D->setScale(curScale,curScale,curScale);

			// Adjust rotation
			particle3D->setRotation(0,rotationCtr * rotationDirection,xRotateVec,yRotateVec,zRotateVec);
			rotationCtr += rotationTween.update();

			// Adjust y-coordinate
			loc.y -= (curScale - prevScale) * 10;
			
			// Check if we're done magnifying
			if (curScale > maxMagnificationSize * 99.0/100.0) {
				bIn3D = true;
				bOkToMagnify = false;
				bMagnifying = false;
			}

			prevScale = curScale;
			return;
		} else if (bOkToDeMagnify) {
			curScale = particleTween.update();
			particle3D->setRotation(0,rotationCtr * rotationDirection,xRotateVec,yRotateVec,zRotateVec);
			rotationCtr += rotationTween.update();
			if (curScale < 1.2f) {
				curScale = 1.0f;
				bOkToDeMagnify = false;
				bIn3D = false;
				particle3D->setRotation(0,0,0,0,0);
			}
			particle3D->setScale(curScale,curScale,curScale);
		}
		
		
		
		// Begin rotating and slow upward speed if in 3D
		if (bIn3D) {
			// Slow down the upward motion since we are in 3D
			loc.y -= 0.8; 

			particle3D->setRotation(0,rotationCtr * rotationDirection,xRotateVec,yRotateVec,zRotateVec);
			rotationCtr += 0.2;
		} else {
			loc.y -= SPEED;
		}

		
		// Initiate the magnification or demagnification events
		if (bOkToMagnify && loc.y < (ofGetHeight() * (4.0 / 4.5)) && loc.y > (ofGetHeight() / 6)) {
			bMagnifying = true;
			particleTween.setParameters(easingQuad, ofxTween::easeOut,2.0f,maxMagnificationSize, 200,0);
			particleTween.start();

			rotationTween.setParameters(easingCirc,ofxTween::easeOut,75.0,0.2,180,0);
			rotationTween.start();
			
		// Detect when we must demagnify
		} else if (!bOkToDeMagnify && loc.y < (ofGetHeight() / 6)) {
			particleTween.setParameters(easingQuad, ofxTween::easeOut,particle3D->scale.x,1.0f, 200, 0);
			particleTween.start();
			
			rotationTween.setParameters(easingCirc,ofxTween::easeOut,0.2,20,180,0);		
			rotationTween.start();
			bOkToDeMagnify = true;
			bIn3D = false;
		}
	}

	
	void draw(){ 
		ofPushMatrix();{
			ofTranslate(loc.x, loc.y);
			ofScale(scale, scale, scale);
			particle3D->draw();
		} ofPopMatrix();
	};
	

	bool alive() {
		return bAlive;
	};
	
	
	ofPoint loc;
	
private:
	bool bAlive;
	float scale;
	
	
	// Magnification
	bool bOkToMagnify;
	bool bOkToDeMagnify;
	bool bMagnifying;
	float maxMagnificationSize;
	float prevScale;
	ofxTween particleTween;
	ofxEasingQuad easingQuad;

	
	// Rotation
	ofxTween rotationTween;
	ofxEasingCirc easingCirc;
	int rotationDirection;
	
	
	// 3D and Rotation
	bool bIn3D;
	ofx3DModelLoader* particle3D;
	float rotationCtr;
	float xRotateVec;
	float yRotateVec;
	float zRotateVec;
};

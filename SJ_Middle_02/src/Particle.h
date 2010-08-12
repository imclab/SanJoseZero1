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

#define SPEED 2
#define MAX_ROTATION_VEC_LENGTH 5


class Particle
{
public:
	
	Particle(){
		bAlive = true;
		
		// 3D and Rotation
		bIn3D = false;
		scale = 1.0f;
//		particle3D.loadModel("hopscotch/hops00.3ds", 10);
		rotationCtr = 0.0;
		xRotateVec = ofRandom(0.0, MAX_ROTATION_VEC_LENGTH);
		yRotateVec = ofRandom(0.0, MAX_ROTATION_VEC_LENGTH);
		zRotateVec = ofRandom(0.0, MAX_ROTATION_VEC_LENGTH);
		float rotationDir = ofRandomuf();
		if (rotationDir < 0.5)
			rotationDirection = -1;
		else
			rotationDirection = 1;


		
//		cout << "New Particle created\n";
		
		
		// Magnification
		prevScale = 0.0;
		bOkToMagnify = true;
		bOkToDeMagnify = false;
		bMagnifying = false;
		maxMagnificationSize = 20.0f;
		particleTween.setParameters(easingBounce1, ofxTween::easeOut,0.25f,maxMagnificationSize, 3000,0);
	}
	
	
	~Particle() {
		delete particle3D;
	}
	
//	int getWidth(){
//		return particle3D->width;
//	}
	
	void setModel( ofx3DModelLoader* _particle3D){
		particle3D = _particle3D;
//		cout << "Model set\n";
	}
	
	void setScale( float _scale){
		scale = _scale;
	}
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	void adjustTweenParams() {
		particleTween.setParameters(easingBounce, ofxTween::easeOut,particle3D->scale.x,1.0f, 20000, 0);
	}
	
	void update(){
		float curScale;
		// First check if we are in the magnification process, which puts everything else on pause.
		if (bMagnifying) {
			//do stuff
			curScale = particleTween.update();
			if (curScale > maxMagnificationSize * 99.0/100.0) {
				bIn3D = true;
				bOkToMagnify = false;
				bMagnifying = false;
			}
			loc.y -= (curScale - prevScale) * 10;
			particle3D->setScale(curScale,curScale,0);
			prevScale = curScale;
			return;
		}
		
		loc.y -= SPEED;
		if (loc.y + ofGetHeight() / 8.0 <= 0) {
			bAlive = false;
			return;
		}
		
		
		// Begin rotating if in 3D
		if (bIn3D) {
			curScale = particleTween.update();
			particle3D->setScale(curScale, curScale, curScale);
			particle3D->setRotation(0,rotationCtr * rotationDirection,xRotateVec,yRotateVec,zRotateVec);
			rotationCtr += 0.2;
		}

		
		// There will be only one magnification event
		if (bOkToMagnify && loc.y < (ofGetHeight() * (12.0 / 15.0)) && loc.y > (ofGetHeight() / 4.5)) {
			bMagnifying = true;
//			if (curScale >= maxMagnificationSize * 99.0/100.0) {
//				curScale = maxMagnificationSize;
//				bOkToMagnify = false;
//				bIn3D = true;
//				particle3D.setRotation(1,0,0,0,0);
//			}
//				bOkToMagnify = false;
			//}
		} else if (!bOkToDeMagnify && loc.y < (ofGetHeight() / 4.5)) {
			adjustTweenParams();
			bOkToDeMagnify = true;
		} else if (bOkToDeMagnify) {
//			curScale = particleTween.update();
			if (curScale < 4.0f) {
				curScale = 1.0f;
				particle3D->setScale(curScale,curScale,curScale);
				bOkToDeMagnify = false;
				bIn3D = false;
				particle3D->setRotation(0,0,0,0,0);
			}
//			setScale(curScale);
		}
		
		
//		cout << "loc.y= " << loc.y << ", scale: " << scale << ", curTween: " << curTween << endl;
	}
	void draw(){ 
		ofPushMatrix();{
			ofTranslate(loc.x, loc.y);
			ofScale(scale, scale, scale);
//			ofRotate(rotationCtr,0,1,1);
//			if (!bIn3D) {
//				img->draw(-getWidth()/2.0,-getWidth()/2.0);
//			} else {
//				glEnable(GL_DEPTH_TEST);
//				glEnable(GL_CULL_FACE);
//				particle3D.draw();
//				glDisable(GL_CULL_FACE);
//				glDisable(GL_DEPTH_TEST);
//			}
//			img->draw(-getWidth()/2.0,-getWidth()/2.0);
//			glFrontFace(GL_CCW);
			//glEnable(GL_CULL_FACE);
			particle3D->draw();
			//glDisable(GL_CULL_FACE)
		} ofPopMatrix();
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint loc;
private:

	// Magnification
	bool bOkToMagnify;
	bool bOkToDeMagnify;
	bool bMagnifying;
	bool bIn3D;
	float prevScale;
	float maxMagnificationSize;
	ofxTween particleTween;
	ofxEasingBounce easingBounce1;
	ofxEasingBounce easingBounce;
	int rotationDirection;
	
	
	// 3D and Rotation
	ofx3DModelLoader* particle3D;
	float rotationCtr;
	float xRotateVec;
	float yRotateVec;
	float zRotateVec;
	
	bool bAlive;
//	ofImage * img;
	float scale;
	
	
};

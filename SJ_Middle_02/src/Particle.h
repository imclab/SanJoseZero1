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


class Particle
{
public:
	
	Particle(){
		bAlive = true;
		bIn3D = false;
		scale = 1.0f;
		particle3D.loadModel("hopscotch/hops00.3ds", 10);
		
		// Magnification
		bOkToMagnify = true;
		maxMagnificationSize = 5.0f;
		particleTween.setParameters(easingBounce, ofxTween::easeOut,0.25f,maxMagnificationSize, 7800,0);
	}
	
	int getWidth(){
		return img->width;
	}
	
	void setImage( ofImage * _img){
		img = _img;
	}
	
	void setScale( float _scale){
		scale = _scale;
	}
	
	void setLoc( float x, float y){
		loc.x = x;
		loc.y = y;
	}
	
	void adjustTweenParams() {
		particleTween.setParameters(easingBounce, ofxTween::easeOut,particle3D.scale.x,0.25f, 1000, 0);
	}
	
	void update(){
		loc.y -= SPEED;
		if (loc.y + img->height <= 0) {
			bAlive = false;
			return;
		}
		
		// Magnification
		float curScale;
		if (bOkToMagnify && loc.y < (ofGetHeight() * (3.0 / 4.0)) && loc.y >= (ofGetHeight() / 4.0)) {
			curScale = particleTween.update();
			if (curScale >= maxMagnificationSize * 99.0/100.0) {
				curScale = maxMagnificationSize;
				bOkToMagnify = false;
				bIn3D = true;
			}
			setScale(curScale);
		} else if (!bOkToMagnify && loc.y < (ofGetHeight() / 4.0)) {
			adjustTweenParams();
			bOkToMagnify = true;
		} else if (bOkToMagnify && loc.y < (ofGetHeight() / 4.0)) {
			curScale = particleTween.update();
			if (curScale < 0.50f) {
				curScale = 0.50f;
				bOkToMagnify = false;
				bIn3D = false;
				particle3D.setRotation(1,0,0,0,0);
			}
			setScale(curScale);
			particle3D.setScale(curScale,curScale,curScale);
		}
		
		if (bIn3D) {
			particle3D.setRotation(1, 270 + ofGetElapsedTimef() * 60, 0, 1, 1);			
		}
		
//		cout << "loc.y= " << loc.y << ", scale: " << scale << ", curTween: " << curTween << endl;
	}
	void draw(){ 
		ofPushMatrix();{
			ofTranslate(loc.x, loc.y);
			ofScale(scale, scale, scale);
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
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			particle3D.draw();
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
		} ofPopMatrix();
	};
	
	bool alive(){ return bAlive; };
	
	ofPoint loc;
private:

	// Magnification
	bool bOkToMagnify;
	bool bIn3D;
	float maxMagnificationSize;
	ofxTween particleTween;
	ofxEasingBounce easingBounce;
	
	
	// 3D
	ofx3DModelLoader particle3D;
	
	
	bool bAlive;
	ofImage * img;
	float scale;
	
	
};

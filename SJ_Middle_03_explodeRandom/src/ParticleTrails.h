/*
 *  ParticleTrails.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 8/30/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once
#include "ofxFBOTexture.h"
#include "ofxShader.h"
#include "Emitter.h"
#include "ofEvents.h"

class ParticleTrails
{
public:
	ParticleTrails(){
		bSetup = false;
		trailsFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
		ofAddListener(ofEvents.windowResized, this, &ParticleTrails::windowResized);
	}
	
	void setup( Emitter * _e ){
		emitter = _e;
	}
	
	void update(){
		trailsFBO.begin();
		ofSetColor(0,0,0,8);
		glDisable(GL_DEPTH_TEST);
		ofRect(0,0,trailsFBO.getWidth(), trailsFBO.getHeight());
		trailsFBO.end();
		ofSetColor(0xffffff);
		
		glEnable(GL_DEPTH_TEST);
		for (int i=0; i<emitter->particles.size(); i++){
			if (emitter->particles[i]->bTransformed){// && emitter->particles[i]->drawType == 1){
				trailsFBO.begin();
				ofPushMatrix();
				emitter->particles[i]->drawTrail();
				ofPopMatrix();
				trailsFBO.end();
			}
		}
		ofSetColor(0xffffff);
	}
	
	void draw(){		
		ofSetColor(255,255,255,200);
		trailsFBO.draw(0, 0);
		ofSetColor(255,255,255,255);
	}
	
	void windowResized( ofResizeEventArgs & e ){
		trailsFBO.allocate(e.width, e.height, GL_RGBA);
	}
	
protected:
	bool bSetup;
	Emitter * emitter;
	ofxFBOTexture trailsFBO;
	
};
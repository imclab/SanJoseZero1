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
		drawMode = 1;
		ofAddListener(ofEvents.windowResized, this, &ParticleTrails::windowResized);
	}
	
	void setDrawMode( int which ){
		drawMode = which;
	}
	
	void setFade( float _particleFade ){
		particleFade = _particleFade;
	}
	
	void setup( Emitter * _e ){
		emitter = _e;
	}
	
	void update(){
		trailsFBO.begin();
		ofSetColor(0,0,0,particleFade);
		glDisable(GL_DEPTH_TEST);
		ofRect(0,0,trailsFBO.getWidth(), trailsFBO.getHeight());
		trailsFBO.end();
		ofSetColor(0xffffff);
		
		glEnable(GL_DEPTH_TEST);
		for (int i=0; i<emitter->particles.size(); i++){
			if ( (drawMode == 0 && emitter->particles[i]->bTransformed) ||
				 (drawMode == 1 && !emitter->particles[i]->bTransformed) ||
				(drawMode == 2))				
			{// && emitter->particles[i]->drawType == 1){
				trailsFBO.begin();
				ofPushMatrix();
				emitter->particles[i]->drawTrail(drawMode);
				ofPopMatrix();
				trailsFBO.end();
			}
		}
		ofSetColor(0xffffff);
	}
	
	void draw(){		
		ofSetColor(255,255,255,255);
		trailsFBO.draw(0, 0);
		ofSetColor(255,255,255,255);
	}
	
	void windowResized( ofResizeEventArgs & e ){
		trailsFBO.allocate(e.width, e.height, GL_RGBA);
	}
	
protected:
	int		drawMode;
	float	particleFade;
	bool	bSetup;
	Emitter * emitter;
	ofxFBOTexture trailsFBO;
	
};
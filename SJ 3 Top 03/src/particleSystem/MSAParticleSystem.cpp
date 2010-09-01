/*
 *  MSAParticleSystem.cpp
 *  ofxMSAFluid Demo
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 *
 */

#include "MSAParticleSystem.h"

using namespace MSA;

MSAParticleSystem::MSAParticleSystem() 
{
	curIndex = 0;
	setWindowSize( Vec2f( 1, 1 ) );
	
    tex.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
    tex.clear();
    drawAlpha = 150;
}

void MSAParticleSystem::windowResized(){
    tex.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA, 4);
    setWindowSize(Vec2f(ofGetWidth(), ofGetHeight()));
};


void MSAParticleSystem::setWindowSize( Vec2f winSize )
{
	windowSize = winSize;
	invWindowSize = Vec2f( 1.0f / winSize.x, 1.0f / winSize.y );
}

void MSAParticleSystem::updateAndDraw( bool drawingFluid ){
	tex.swapIn();
    ofEnableAlphaBlending();
    ofSetColor(0,0,0, 10);
    ofRect(0,0,ofGetWidth(), ofGetHeight());
    ofSetColor(0xffffff);
	
	glEnable(GL_LINE_SMOOTH);       
	
	for(int i=0; i<MAX_PARTICLES; i++) {
		if(msaParticles[i].alpha > 0) {
			msaParticles[i].update( *solver, windowSize, invWindowSize );
			msaParticles[i].updateVertexArrays( drawingFluid, invWindowSize, i, posArray, colArray);
		}
	}    
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, posArray);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colArray);
	
	glDrawArrays(GL_LINES, 0, MAX_PARTICLES * 2);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
    tex.swapOut();
    
    //ofEnableAlphaBlending();
    //glEnable( GL_TEXTURE_2D );
    //glBlendFunc(GL_ONE,GL_ONE);
    ofSetColor(255,255,255, drawAlpha);
    tex.draw(0,0);
}


void MSAParticleSystem::addParticles( const Vec2f &pos, int count ){
	for(int i=0; i<count; i++)
		addParticle( pos + Rand::randVec2f() * 15 );
}


void MSAParticleSystem::addParticle( const Vec2f &pos ) {
	msaParticles[curIndex].init( pos.x, pos.y );
	curIndex++;
	if(curIndex >= MAX_PARTICLES) curIndex = 0;
}

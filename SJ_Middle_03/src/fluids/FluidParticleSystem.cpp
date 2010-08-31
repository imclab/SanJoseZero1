/*
 *  FluidParticleSystem.cpp
 *  ofxMSAFluid Demo
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 *
 */

#include "FluidParticleSystem.h"

using namespace MSA;

FluidParticleSystem::FluidParticleSystem() 
{
	curIndex = 0;
	setWindowSize( Vec2f( 1, 1 ) );
	min = max = 0;
}

bool FluidParticleSystem::setWindowSize( Vec2f winSize )
{
	bool bNew = true;
	if (windowSize == winSize) bNew = false;
	windowSize = winSize;
	invWindowSize = Vec2f( 1.0f / winSize.x, 1.0f / winSize.y );
	return bNew;
}

bool FluidParticleSystem::setBuffer( float top, float bottom ){
	if (min == top && max == bottom) return false;
	
	min = top;
	max = bottom;
	
	for (int i=0; i<MAX_PARTICLES; i++){
		//if (msaParticles[i].bAlive){
			msaParticles[i].min = top;
			msaParticles[i].max = bottom;	
		//};
	}
	
	return true;
};

void FluidParticleSystem::updateAndDraw( bool drawingFluid ){
	glEnable(GL_BLEND);
	glDisable( GL_TEXTURE_2D );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);       
	
	for(int i=0; i<MAX_PARTICLES; i++) {
		if(msaParticles[i].alpha > 0.01) {
			msaParticles[i].update( *solver, windowSize, invWindowSize );
			msaParticles[i].updateVertexArrays( drawingFluid, invWindowSize, i, posArray, colArray);
		} else {
			int vi = i * 4;
			
			posArray[vi++] = -1;
			posArray[vi++] = -1;
			posArray[vi++] = -1;
			posArray[vi++] = -1;

			int ci = i * 6;
			colArray[ci++] = 0;
			colArray[ci++] = 0;
			colArray[ci++] = 0;
			colArray[ci++] = 0;
			colArray[ci++] = 0;
			colArray[ci++] = 0;			
		};
	}    
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, posArray);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colArray);
	
	glDrawArrays(GL_LINES, 0, MAX_PARTICLES * 2);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glDisable(GL_BLEND);
}


void FluidParticleSystem::addParticles( const Vec2f &pos, int count ){
	for(int i=0; i<count; i++)
		addParticle( pos + Rand::randVec2f() * 15 );
}


void FluidParticleSystem::addParticle( const Vec2f &pos ) {
	msaParticles[curIndex].init( pos.x, pos.y );
	curIndex++;
	if(curIndex >= MAX_PARTICLES) curIndex = 0;
}

/*
 *  MSAParticleSystem.h
 *  ofxMSAFluid Demo
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 *
 */
#pragma once

#include "MSAParticle.h"

#define MAX_PARTICLES		50000

using namespace MSA;

class MSAParticleSystem {
public:	
	
    float posArray[MAX_PARTICLES * 2 * 2];
    float colArray[MAX_PARTICLES * 3 * 2];
	Vec2f				windowSize;
	Vec2f				invWindowSize;
	const FluidSolver	*solver;
	
    int curIndex;
	
    MSAParticle msaParticles[MAX_PARTICLES];
	
	MSAParticleSystem();
	void setFluidSolver( const FluidSolver *aSolver ) { solver = aSolver; }
	
    void updateAndDraw( bool drawingFluid );
	void addParticles( const Vec2f &pos, int count );
	void addParticle( const Vec2f &pos );
	void setWindowSize( Vec2f winSize );
};


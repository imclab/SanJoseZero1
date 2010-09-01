/*
 *	based on MSAParticleSystem.h
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 */

/*
 *  FluidParticleSystem.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 8/30/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "MSAParticle.h"

#define MAX_PARTICLES		50000

using namespace MSA;

class FluidParticleSystem {
public:	
	
    float posArray[MAX_PARTICLES * 2 * 2];
    float colArray[MAX_PARTICLES * 3 * 2];
	Vec2f				windowSize;
	Vec2f				invWindowSize;
	const FluidSolver	*solver;
	
    int curIndex;
	
	float max, min;
	
    MSAParticle msaParticles[MAX_PARTICLES];
	
	FluidParticleSystem();
	void setFluidSolver( const FluidSolver *aSolver ) { solver = aSolver; }
	
    void updateAndDraw( bool drawingFluid );
	void addParticles( const Vec2f &pos, int count );
	void addParticle( const Vec2f &pos );
	bool setWindowSize( Vec2f winSize );
	bool setBuffer( float top, float bottom);
};


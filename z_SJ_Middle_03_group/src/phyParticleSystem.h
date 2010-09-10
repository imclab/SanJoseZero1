/*
 *  phyParticleSystem.h
 *  SJ_Transformer
 *
 *  Created by Zach Gage on 8/25/10.
 *  Copyright 2010 stfj. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "Emitter.h"
#include "particle3D.h"
#include "ofxFBOTexture.h"
#include "LineExplosion.h"

class phyParticleSystem
{
public:
	phyParticleSystem(Emitter * _emitter);
	
	void update();
	void draw();
	void explode( Particle * particle );
	
	Emitter * emitter;
	vector<particle3D *> particles;
	vector<LineExplosion *> explosions;
	
protected:
	//ofxFBOTexture trailTex;
};
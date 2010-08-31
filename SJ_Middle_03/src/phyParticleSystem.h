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
#include "phyParticle.h"
#include "ShapeBatchRenderer.h"
#include "ofxFBOTexture.h"

class phyParticleSystem
{
public:
	phyParticleSystem(Emitter * _emitter);
	
	void update();
	void draw();
	
	
	Emitter * emitter;
	vector<phyParticle *> particles;
	
	ShapeBatchRenderer * renderer;
	
protected:
	void drawParticles();
	void cleanUpParticles();
	ofxFBOTexture trails;
};
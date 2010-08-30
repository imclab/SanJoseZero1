/*
 *  phyParticleSystem.cpp
 *  SJ_Transformer
 *
 *  Created by Zach Gage on 8/25/10.
 *  Copyright 2010 stfj. All rights reserved.
 *
 */

#include "phyParticleSystem.h"

phyParticleSystem::phyParticleSystem(Emitter * _emitter)
{
	emitter = _emitter;
	
	renderer = new ShapeBatchRenderer(SBR_TRIANGLE, 100000, 1);
}


void phyParticleSystem::update()
{
	renderer->clear();
	
	
	for(int i=0;i<emitter->particles.size();i++)
	{
		if(emitter->particles[i]->bTransforming)
		{
			for(int p=0;p<10;p++)
			{
				phyParticle * p;
				
				p = new phyParticle();
				
				p->setInitialCondition(500,500, ofRandomf()*5, ofRandomf()*5);
			
				
				
				//p->setInitialCondition(emitter->particles[i]->loc.x, emitter->particles[i]->loc.y, ofRandomf()*5, ofRandomf()*5);
				
				particles.push_back(p);
			}
		}
	}
	
	
	cleanUpParticles();
	drawParticles();
}


void phyParticleSystem::draw()
{
	renderer->draw();
}




void phyParticleSystem::drawParticles()
{
	for(int i=0;i<particles.size();i++)
	{
		renderer->setColor(255,255,255,255);
		renderer->addCircle(particles[i]->pos.x, particles[i]->pos.y, 0, 2);
	}
}

void phyParticleSystem::cleanUpParticles()
{
	//check to remove particles
}
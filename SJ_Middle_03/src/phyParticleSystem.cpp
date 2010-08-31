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
	trails.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
}


void phyParticleSystem::update()
{
	renderer->clear();
	
	
	for(int i=0;i<emitter->particles.size();i++)
	{
		if(emitter->particles[i]->bTransforming && emitter->particles[i]->index == 0)
		{
			
			if (ofRandomuf()*10 > 8){
				
				float angleAdd = 10 + ofRandomuf()*30.;
				
				float angle = ofRandom(0,360.);
				
				for(int j=0;j<60;j++)
				{
					phyParticle * p;
					
					p = new phyParticle();
					
					ofPoint aVel;
					aVel.x =.5f * sin(angle);
					aVel.y = .5f * cos(angle);
						
					angle += angleAdd;
					
					p->setInitialCondition(emitter->particles[i]->getLoc().x, emitter->particles[i]->getLoc().y, aVel.x, aVel.y);
					p->color.r = ofRandomuf()*255.;
					p->color.g = ofRandomuf()*255.;
					p->color.b = ofRandomuf()*255.;
					p->mass = fabs(emitter->particles[i]->getVelocity().x/3.0f);
					p->pos.z = emitter->particles[i]->getLoc().z;
					
					p->addForce(aVel.x, aVel.y);
					
					particles.push_back(p);
				}
			}
		}
	}
	
	
	cleanUpParticles();
	drawParticles();
	//trails.begin();
	ofSetColor(0,0,0,10);
	ofRect(0,0,trails.getWidth(), trails.getHeight());
	ofSetColor(0xffffff);
	//renderer->draw();
	//trails.end();
}


void phyParticleSystem::draw()
{
	renderer->draw();
	//trails.draw(0,0);
}




void phyParticleSystem::drawParticles()
{
	for(int i=0;i<particles.size();i++)
	{
		renderer->setColor(particles[i]->color.r,particles[i]->color.g,particles[i]->color.b,particles[i]->vel.x);
		renderer->addCircle(particles[i]->pos.x, particles[i]->pos.y, particles[i]->pos.z, particles[i]->mass-=.1);
		particles[i]->addForce(ofRandomf()/10.0f, ofRandomf()/10.0f);
		particles[i]->update();
		//particles[i]->bounceOffWalls();
	}
}

void phyParticleSystem::cleanUpParticles()
{
	while(particles.size() > 10000){
		particles.erase(particles.begin());
	}
	
	
	for(int i=particles.size()-1;i>=0;i--)
	{
		if (particles[i]->mass < .1) particles.erase(particles.begin()+i);
	}
	
	
	//check to remove particles
}
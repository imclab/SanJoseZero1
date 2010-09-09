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
}


void phyParticleSystem::update()
{	
	for(int i=0;i<emitter->particles.size();i++)
	{
		if(emitter->particles[i]->bTransforming && 
		   !emitter->particles[i]->bExploded && 
		   emitter->particles[i]->getLoc().y < (emitter->particles[i]->getExplodeLoc()) &&
		   emitter->particles[i]->getLoc().x > 0 && emitter->particles[i]->getLoc().x < ofGetWidth())// && emitter->particles[i]->index == 0)
		{
			explode( emitter->particles[i] );
		}
	}
	
	for(int i=particles.size()-1;i>=0;i--)
	{		
		particles[i]->update();
		
		//force fade if below thresholds
		if (particles[i]->pos.y > emitter->getTransformStart() || particles[i]->pos.y < emitter->getTransformEnd()){
			//particles[i]->a *= .85;
		}		
		if (!particles[i]->bAlive && explosions[i]->a <= .1f){
			particle3D * p = particles[i];
			LineExplosion * l = explosions[i];
			particles.erase(particles.begin()+i);
			explosions.erase(explosions.begin()+i);
			delete p;
			delete l;
		}
	}
}


void phyParticleSystem::draw()
{
	ofEnableAlphaBlending();
	
	glDisable(GL_DEPTH_TEST);
	ofxLightsOff();
	ofSetColor(0xffffff);
	for (int i=0; i<explosions.size(); i++){
		explosions[i]->draw();
	}
	ofxLightsOn();
	//glEnable(GL_DEPTH_TEST);
	for (int i=0; i<particles.size(); i++){
		if (particles[i]->bAlive){
			particles[i]->draw();
		}
	};
	glDisable(GL_DEPTH_TEST);
	ofxLightsOff();
	
}

void phyParticleSystem::explode( Particle * particle ){
	particle->bExploded = true;
	particle->startTransform();
	
	float angleAdd = 10 + ofRandomuf()*30.;				
	float angle = ofRandom(0,360.);				
	float toEmit = ofRandom(5, 50);
	
	for(int j=0; j<toEmit; j++)
	{
		particle3D * p;
		
		p = new particle3D();
		
		ofPoint aVel;
		aVel.x = fmax(ofRandom(1.0, 5.0f), fabs(particle->getVelocity().x / ofRandom(1.0f, 15.0f))) * sin(angle);
		aVel.y = fmax(ofRandom(1.0, 5.0f), fabs(particle->getVelocity().x / ofRandom(1.0f, 15.0f))) * cos(angle);
		
		angle += angleAdd;
		
		particle->targetScale *= ofRandom(1.01, 1.05f);
		particle->rotation += ofRandom(-60,60);
		particle->vel *= -2;
		
		p->setInitialCondition(particle->getLoc().x, particle->getLoc().y, particle->getLoc().z, aVel.x, aVel.y, ofRandom(-5.0f, 5.0f));
		p->setColor(particle->color);
		
		p->mass = fabs(particle->getVelocity().x/3.0f);
		p->pos.z = particle->getLoc().z;
		
		if (p->type == 0){
			p->addForce(aVel.x*ofRandom(.2, .8), aVel.y*ofRandom(.2, .8));
		} else {
			p->addForce(aVel.x, aVel.y);
		}
		p->setScale( particle->getHeight(), particle->getHeight() );
		
		LineExplosion * explosion = new LineExplosion();
		explosion->x = particle->getLoc().x;
		explosion->y = particle->getLoc().y;
		explosion->setColor( particle->color );
		explosions.push_back(explosion);
		
		//adjust damping alpha based on number of particles
		if (emitter->particles.size() < 5){
			explosion->alphaDamping += .005;
			p->alphaDamping += .005;
		} else if (particle->numNeighbors() > 3 || emitter->particles.size() > 20){
			p->alphaDamping -= .001;
			explosion->alphaDamping -= .001;
		}
		
		particle->dampenVelocity(-.5f);
		
		particles.push_back(p);
		angleAdd = 10 + ofRandomuf()*30.;
	}
	
};
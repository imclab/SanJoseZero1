/*
 *  particle3D.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 9/1/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "phyParticle.h"
#include "MSAShape3D.h"
#include "ofx3DUtils.h"

using namespace MSA;

class particle3D : public phyParticle
{
public:
	
	ofxVec3f ranRotation;
	float rotSpeed;
	float r, g, b, a;
	ofPoint scale;
	ofPoint startScale, targetScale;
	
	particle3D(){
		targetScale.x = ofRandom(5., 40.);
		targetScale.y = ofRandom(5., 40.);
		r = ofRandomf()*255.;
		g = ofRandomf()*255.;
		b = ofRandomf()*255.;
		a = 255;
		
		phyParticle::phyParticle();
		ranRotation.x = ofRandomf()*360.f;
		ranRotation.y = ofRandomf()*360.f;
		ranRotation.z = ofRandomf()*360.f;
		
		rotSpeed = ofRandomf()*10.;
		ofSetColor(0xffffff);
		damping = .85f;
		startScale.x = 0;
		startScale.y = 0;
		scale = startScale;
		type = (int) floor(ofRandom(0,5));
	}
	
	ofPoint startPoint;
	
	void setInitialCondition(float px, float py, float pz, float vx, float vy, float vz=0){
		startPoint.x = px;
		startPoint.y = py;
		startPoint.z = pz;		
		
		phyParticle::setInitialCondition(px, py, pz, vx, vy, vz);
	};
	
	void setScale( float x, float y ){
		targetScale.x = x/(2 + ofRandomuf()*4.);
		targetScale.y = y/(2 + ofRandomuf()*4.);
		if (type >= 1){
			targetScale.x *= ofRandom(1., 2.f);
		}
	};
	
	void setColor (ofColor color){
		setColor(color.r, color.g, color.b);
	}
	
	void setColor( float _r, float _g, float _b){
		r = _r;
		g = _g;
		b = _b;
		
		float ran = ofRandom(-10.0f, 10.0f);
		
		cout<<"uhh "<<ran<<endl;
		
		r += ran;
		ran = ofRandom(-10.0f, 10.0f);
		g += ran;
		ran = ofRandom(-10.0f, 10.0f);
		b += ran;
		
		/*r *= 3.f;
		g *= 3.f;
		b *= 3.f;*/
	};
	
	void update(){
		
		phyParticle::update();
		frc *= damping;
		rotSpeed *= .91;
		vel.x -= vel.x/10.f;
		vel.y -= vel.y/10.f;
		vel.z -= vel.z/10.f;
		
		ranRotation.x += rotSpeed;
		ranRotation.y += rotSpeed;
		ranRotation.z += rotSpeed;
	};
	
	void draw(){
			
		rot+=rotChange;
		
		ofSetColor(r,g,b,a);
		
		ofSetRectMode(OF_RECTMODE_CENTER);
		glPushMatrix();
		
		ofxLightsOff();
		if (type >= 1){
			glBegin(GL_POLYGON);
			glVertex3f(startPoint.x, startPoint.y, startPoint.z);
			glVertex3f(pos.x-scale.x, pos.y, pos.z-scale.x);
			glVertex3f(pos.x+scale.x, pos.y, pos.z+scale.x);
			glEnd();
		}
		ofxLightsOn();
		
		glTranslated(pos.x,pos.y,pos.z);
		glRotatef(rot,0,0,1);
		ofRotateX(ranRotation.x);
		ofRotateY(ranRotation.y);
		ofRotateZ(ranRotation.z);
		
		if (type == 99){
			ofxCone(0,0,0,scale.x/2.0f, scale.y, scale.x);
		} else if (type == 10000){
			ofxCapsule(0,0,0,scale.x, scale.y, scale.x);
		} else if (type == 0){
			ofxBox(0,0,0,scale.x, scale.y, scale.x );
		}
		scale.x -= (scale.x - targetScale.x)/20.0f;
		scale.y -= (scale.y - targetScale.y)/20.0f;
		scale.z -= (scale.z - targetScale.z)/20.0f;
		
		a *= .989;
		if (a <=30){
			bAlive = false;
		}
		
		glLineWidth(1);
		glPopMatrix();
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(0xffffff);
	};
	
};
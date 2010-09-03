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
#include "ofx3DUtils.h"
#include "ofxColor.h"

class particle3D : public phyParticle
{
public:
	
	ofxVec3f ranRotation;
	float rotSpeed;
	ofxColor fillColor;
	ofPoint scale;
	ofPoint startScale, targetScale;
	float alphaDamping;
	
	int age, life;
	
	float scaleSpeed;
	
	particle3D(){
		targetScale.x = ofRandom(5., 40.);
		targetScale.y = ofRandom(5., 40.);
		
		fillColor.setColorRange(255.f);
		fillColor.hue = ofRandomf()*255.;
		
		phyParticle::phyParticle();
		ranRotation.x = ofRandomf()*360.f;
		ranRotation.y = ofRandomf()*360.f;
		ranRotation.z = ofRandomf()*360.f;
		
		rotSpeed = ofRandomf()*30.;
		ofSetColor(0xffffff);
		damping = .85f+ofRandom(-.1, .1);
		startScale.x = 0;
		startScale.y = 0;
		scale = startScale;
		type = (int) floor(ofRandom(0,3));
		alphaDamping = .989;
		
		age = 0;
		life = ofRandom(30, 140);
		scaleSpeed = ofRandom( 5, 30);
	}
		
	ofPoint startPoint;
	
	void setInitialCondition(float px, float py, float pz, float vx, float vy, float vz=0){
		startPoint.x = px;
		startPoint.y = py;
		startPoint.z = pz;		
		
		if (type == 0){
			vx *= ofRandom(.2, .75);
			vy *= ofRandom(.2, .75);
			vx *= ofRandom(.2, .75);
		}
		
		phyParticle::setInitialCondition(px, py, pz, vx, vy, vz);
	};
	
	void setScale( float x, float y ){
		targetScale.x = x/(1.3 + ofRandomuf()*4.);
		targetScale.y = y/(1.3 + ofRandomuf()*4.);
		if (type >= 1){
			targetScale.x *= ofRandom(1., 2.f);
		}
	};
	
	void setColor (ofColor color){
		setColor(color.r, color.g, color.b);
	}
	
	void setColor( float _r, float _g, float _b){
		fillColor.red = _r;
		fillColor.green = _g;
		fillColor.blue = _b;
		fillColor.updateHsv();
		
		fillColor.hue *= ofRandom(.8, 1.2f);
		fillColor.value *= ofRandom(.8, 1.2f);
		fillColor.saturation *= ofRandom(.8, 1.2f);
		
		fillColor.updateRgb();
		
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
		
		if (pos.y < 0) pos.y = 0;
		else if (pos.y > ofGetHeight()) pos.y = ofGetHeight();
	};
	
	void draw(){
			
		rot+=rotChange;
		
		ofSetColor(fillColor.red, fillColor.green, fillColor.blue,fillColor.alpha);
		
		ofSetRectMode(OF_RECTMODE_CENTER);
		glPushMatrix();
		
		ofxLightsOff();
		if (type >= 1){
			ofTriangle(startPoint.x, startPoint.y, startPoint.z, pos.x-scale.x, pos.y, pos.z-scale.x, pos.x+scale.x, pos.y, pos.z+scale.x);
			/*glVertex3f(startPoint.x, startPoint.y, startPoint.z);
			glVertex3f(pos.x-scale.x, pos.y, pos.z-scale.x);
			glVertex3f(pos.x+scale.x, pos.y, pos.z+scale.x);*/
		}
		ofxLightsOn();
		
		glTranslated(pos.x,pos.y,pos.z);
		glRotatef(rot,0,0,1);
		ofRotateX(ranRotation.x);
		ofRotateY(ranRotation.y);
		ofRotateZ(ranRotation.z);
		
		//if (type == 99){
//			ofxCone(0,0,0,scale.x/2.0f, scale.y, scale.x);
//		} else if (type == 10000){
//			ofxCapsule(0,0,0,scale.x, scale.y, scale.x);
//		} else 
		
		if (type == 0){
			ofxBox(0,0,0,scale.x, scale.y, scale.x );
		}
		scale.x -= (scale.x - targetScale.x)/scaleSpeed;
		scale.y -= (scale.y - targetScale.y)/scaleSpeed;
		scale.z -= (scale.z - targetScale.z)/scaleSpeed;
		
		fillColor.alpha *= alphaDamping;
		if (fillColor.alpha <=50){
			bAlive = false;
			fillColor.alpha *= .75;
		}
		
		glLineWidth(1);
		glPopMatrix();
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(0xffffff);
	};
	
};
/*
 *  particle3D.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 9/1/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 
 THIS CLASS IS BOTH 3D CUBES AND "WEDGES"
 
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
	vector<ofQuad> quads;
	float alphaDamping;
	
	int age, life, lastAdded;
	ofPoint lastPoint[2];
	float distance;
	bool bAdd;
	
	float lineWidth;
	
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
		type = (int) floor(ofRandom(0,4));
		alphaDamping = .989;
		
		age = 0;
		lastAdded = 10;
		bAdd = false;
		
		life = ofRandom(30, 140);
		scaleSpeed = ofRandom( 5, 30);
		lineWidth = ofRandom(5,20.0f);
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
	
		lastPoint[1].x = startPoint.x-scale.x;
		lastPoint[0].x = startPoint.x+scale.x;
		lastPoint[0].y = lastPoint[1].y = startPoint.y;
		lastPoint[1].z = startPoint.z - scale.x;
		lastPoint[0].z = startPoint.z + scale.x;
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
		
		fillColor.hue *= ofRandom(.99f, 1.05f);
		fillColor.value = 400;//ofRandom(1.01f, 1.1f);
		fillColor.saturation = 400;//;ofRandom(.99, 1.2f);
		
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
		
		if (type >= 3){
			//lastAdded = 10;
			
			float dirX = (float) (pos.x - startPoint.x)/ofGetWidth();
			float dirY = (float) (pos.y - startPoint.y)/ofGetHeight();
			
			ofxVec2f dumPos = pos;
			dumPos.x += dirX * scale.x*2.f;
			dumPos.y += dirY * scale.x*2.f;
			
			ofxVec2f p1 = dumPos;				
			ofxVec2f p2 = dumPos;
			
			float angle = atan((pos.y - startPoint.y)/(pos.x - startPoint.x))*(180.f/PI);
			
			ofxVec2f dum1, dum2;
			
			dum1 = p1;
			dum2 = p2;
			
			p1.rotate(angle-90, pos);
			p2.rotate(-(angle-90), pos);
			
			float dist = ofDist(pos.x, pos.y, lastPoint[1].x, lastPoint[1].y);
			
			//if (dist > 50.0f){
				float increment = dist/lineWidth;
				float yadd = increment;
				increment = floor(increment);
				
				for (int i=0; i<increment; i+=4){	
					if (i + 2 <increment){
						ofQuad newQuad;
						newQuad.points[0].x = ofLerp(lastPoint[0].x, p2.x, i/increment); //lastPoint[1].x;
						newQuad.points[0].y = ofLerp(lastPoint[0].y, p2.y, i/increment); //lastPoint[1].y;
						//newQuad.points[0].z = ofLerp(lastPoint[0].z, pos.z+scale.x, i/increment); //lastPoint[1].z;
						newQuad.points[1].x = ofLerp(lastPoint[1].x, p1.x, i/increment); //lastPoint[0].x;
						newQuad.points[1].y = ofLerp(lastPoint[1].y, p1.y, i/increment); //lastPoint[0].y;
						//newQuad.points[1].z = ofLerp(lastPoint[1].z, pos.z-scale.x, i/increment); //lastPoint[0].z;
						
						newQuad.points[3].x = ofLerp(lastPoint[0].x, p1.x, (i+2)/increment); //lastPoint[1].x;
						newQuad.points[3].y = ofLerp(lastPoint[0].y, p1.y, (i+2)/increment); //lastPoint[1].y;
						//newQuad.points[3].z = ofLerp(lastPoint[0].z, pos.z+scale.x, (i+2)/increment); //lastPoint[1].z;
						newQuad.points[2].x = ofLerp(lastPoint[1].x, p2.x, (i+2)/increment); //lastPoint[0].x;
						newQuad.points[2].y = ofLerp(lastPoint[1].y, p2.y, (i+2)/increment); //lastPoint[0].y;
						//newQuad.points[2].z = ofLerp(lastPoint[1].z, pos.z-scale.x, (i+2)/increment); //lastPoint[0].z;
						quads.push_back(newQuad);
					}					
				}
			//}
			if (increment > 2){
				lastPoint[0].x = p1.x;
				lastPoint[0].y = p1.y;
				lastPoint[1].x = p2.x;
				lastPoint[1].y = p2.y;
				//lastPoint[1].z = pos.z - scale.x;
				//lastPoint[0].z = pos.z + scale.x;
			}			
		};
	};
	
	void draw(){
			
		rot+=rotChange;
		
		ofSetColor(fillColor.red, fillColor.green, fillColor.blue,fillColor.alpha);
		
		ofSetRectMode(OF_RECTMODE_CENTER);
		glPushMatrix();{		
			ofxLightsOff();
			if (type >= 3){
				for (int i=0; i<quads.size(); i++){
					quads[i].draw();
				};
			} else {
				pos.z = 0;
				
				float dirX = (float) (pos.x - startPoint.x)/ofGetWidth();
				float dirY = (float) (pos.y - startPoint.y)/ofGetHeight();
				
				ofxVec2f dumPos = pos;
				dumPos.x += dirX * scale.x *2.f;
				dumPos.y += dirY * scale.x *2.f;
								
				ofxVec2f p1 = dumPos;				
				ofxVec2f p2 = dumPos;
				
				float angle = atan((pos.y - startPoint.y)/(pos.x - startPoint.x))*(180.f/PI);
								
				ofxVec2f dum1, dum2;
				
				dum1 = p1;
				dum2 = p2;
				
				p1.rotate(angle-90, pos);
				p2.rotate(-(angle-90), pos);
				
				ofTriangle(startPoint.x, startPoint.y, startPoint.z, p1.x, p1.y, 0, p2.x, p2.y, 0);
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
		} glPopMatrix();
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetColor(0xffffff);
	};
	
};
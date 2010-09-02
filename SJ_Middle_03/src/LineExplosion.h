/*
 *  LineExplosion.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 9/1/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"

class LineExplosion : public ofRectangle 
{
public:
	
	int age, life;
	int complexity;
	float startAngle;
	float endRadius, currentRadius;
	float a;
	
	int stippleVal;
	
	LineExplosion(){
		age = 0;
		life = ofRandom(5, 100);
		complexity = ofRandom(5,50);
		startAngle = ofRandom(0,360);
		endRadius = ofRandom(50,300);
		currentRadius = 0;
		a = 1.0f;
		stippleVal = 0x0000;
		stippleVal += ofRandom(1, 0xFF);
	}
	
	~LineExplosion(){
		
	};
	
	ofColor color, color2;
	
	void setColor( ofColor c ){
		color = c;
		color2.r = 255 - color.r;
		color2.g = 255 - color.g;
		color2.b = 255 - color.b;
	}
	
	void draw(){
		//glLineStipple(1, 0xaaaaaa);
		//glLineStipple (1, stippleVal);
		glEnable(GL_LINE_STIPPLE);
		glEnable(GL_LINE_SMOOTH);
		
		ofSetColor(0xffffff);
		glBegin(GL_LINES);
		float angleAdd = 360.f/complexity;
		float angle = startAngle;
		
		for (int i=0; i<complexity; i++){
			
			ofPoint toPoint;
			toPoint.x = x + currentRadius * sin(angle+4);
			toPoint.y = y + currentRadius * cos(angle+4);
						
			glColor4f( color2.r/25.f, color2.g/25.f, color2.b/25.f, a);
			glVertex2f( x+1, y+1);
			glVertex2f(toPoint.x, toPoint.y);
			
			toPoint.x = x + currentRadius * sin(angle);
			toPoint.y = y + currentRadius * cos(angle);
						
			glColor4f( color.r/25.f, color.g/25.f, color.b/25.f, a);
			glVertex2f( x, y);
			glVertex2f(toPoint.x, toPoint.y);
						
			angle += angleAdd;	
			currentRadius = (float) age*endRadius/life;
			if (currentRadius > endRadius ) currentRadius = endRadius;
		}
		glEnd();		
		glDisable(GL_LINE_STIPPLE);

		ofSetColor(0xffffff);
		
		age++;
		a *= .989f;
		if (age > life){
			a *= .5;
		}
	};
};
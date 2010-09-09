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
#define SIZEOF_FLOAT				4

class LineExplosion : public ofRectangle 
{
public:
	
	int age, life;
	int complexity;
	float startAngle;
	float endRadius, currentRadius;
	float a;
	
	int stippleVal;
	
	float alphaDamping;
	float * vertices;
	float * colors;
	
	LineExplosion(){
		age = 0;
		life = ofRandom(5, 100);
		complexity = ofRandom(5,30);
		vertices = colors = 0;
		vertices = (float *) realloc(vertices, 2 * 4 * complexity * SIZEOF_FLOAT);
		colors = (float *) realloc(colors, 4 * 4 * complexity * SIZEOF_FLOAT);
				
		startAngle = ofRandom(0,360);
		endRadius = ofRandom(50,300);
		currentRadius = 0;
		a = 1.0f;
		stippleVal = 0x0000;
		stippleVal += ofRandom(1, 0xFF);
		alphaDamping = .995;
		//glLineStipple (1, stippleVal);
	}
	
	~LineExplosion(){
		free(vertices);
		free(colors);
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
		
		//ofEnableSmoothing();
		ofEnableAlphaBlending();
		ofSetColor(0xffffff);
		float angleAdd = 360.f/complexity;
		float angle = startAngle;
		
		if (vertices == NULL) cout<<"NO!"<<endl;
		
		
		for (int i=0; i<complexity; i++){
			
			int which = i*8;
			
			ofPoint toPoint;
			toPoint.x = x + currentRadius * sin(angle+4);
			toPoint.y = y + currentRadius * cos(angle+4);
						
			vertices[which]		= (float) x;
			vertices[which+1]	= (float) y;
			vertices[which+2]	= (float) toPoint.x;
			vertices[which+3]	= (float) toPoint.y;			 
			
			colors[which] = (float) color2.r/255.f;
			colors[which+1] = (float) color2.g/255.f;
			colors[which+2] = (float) color2.b/255.f;
			colors[which+3] = (float) 0.0f;
			
			colors[which+4] = (float) color2.r/255.f;
			colors[which+5] = (float) color2.g/255.f;
			colors[which+6] = (float) color2.b/255.f;
			colors[which+7] = (float) a;
			
			//glColor4f( color2.r/255.f, color2.g/255.f, color2.b/255.f, a);
			//ofLine(x,y, toPoint.x, toPoint.y);
			
			toPoint.x = x + currentRadius * sin(angle);
			toPoint.y = y + currentRadius * cos(angle);
						
			//glColor4f( color.r/25.f, color.g/25.f, color.b/25.f, a);
			//ofLine(x,y, toPoint.x, toPoint.y);
					
			vertices[which+4]	= (float) x;
			vertices[which+5]	= (float) y;
			vertices[which+6]	= (float) toPoint.x;
			vertices[which+7]	= (float) toPoint.y;			 
			
			colors[which+8] = (float) color.r/255.f;
			colors[which+9] = (float) color.g/255.f;
			colors[which+10] = (float) color.b/255.f;
			colors[which+11] = (float) 0.0;
			
			colors[which+12] = (float) color.r/255.f;
			colors[which+13] = (float) color.g/255.f;
			colors[which+14] = (float) color.b/255.f;
			colors[which+15] = (float) a;
			
			angle += angleAdd;	
			currentRadius = (float) age*endRadius/life;
			if (currentRadius > endRadius ) currentRadius = endRadius;
		}
		
		glLineWidth(2);
		glEnable(GL_LINE_STIPPLE);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &vertices[0]);
		glColorPointer(4, GL_FLOAT, 0, &colors[0]);
		glDrawArrays(GL_LINES, 0, 2);	
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);	
		glDisable(GL_LINE_STIPPLE);
		glLineWidth(1);
		
		ofSetColor(0xffffff);
		
		age++;
		a *= alphaDamping;
		if (age > life){
			a *= .75;
		}
	};
};
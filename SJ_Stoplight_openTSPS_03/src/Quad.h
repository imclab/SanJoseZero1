/*
 *  Quad.h
 *  Hopscotch Confetti
 *
 *  Created by Brett Renfer on 3/5/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#define DEBUG true

class Quad
{	
public:
	ofPoint p1, p2, p3, p4;
	bool active;
	bool pressed;
	bool selected[4];
	vector <ofPoint *> points;
	string name;
	int index;
	string address; // OSC address for the quad

	Quad()
	: name(""){
		p1 = ofPoint(0,0);
		p2 = ofPoint(0,0);
		p3 = ofPoint(0,0);
		p4 = ofPoint(0,0);
		for (int i=0; i<4; i++){
			selected[i] = false; 
		}
		pressed = false;
		active = false;
		index = -1;
	}
	
	Quad(string name, int index, string _address)
	: name(name), 
	index(index){ 
		p1 = ofPoint(0,0);
		p2 = ofPoint(0,0);
		p3 = ofPoint(0,0);
		p4 = ofPoint(0,0);
		for (int i=0; i<4; i++){
			selected[i] = false; 
		}
		address = _address;
		pressed = false;
		active = false;
	};
	
	vector <ofPoint> getNormalizedPoints( int camWidth, int camHeight){
		vector <ofPoint> pts;
		ofPoint p_1;
		p_1.x = p1.x/camWidth;
		p_1.y = p1.y/camHeight;
		pts.push_back(p_1);
		
		ofPoint p_2;
		p_2.x = p2.x/camWidth;
		p_2.y = p2.y/camHeight;
		pts.push_back(p_2);
		
		ofPoint p_3;
		p_3.x = p3.x/camWidth;
		p_3.y = p3.y/camHeight;
		pts.push_back(p_3);
		
		ofPoint p_4;
		p_4.x = p4.x/camWidth;
		p_4.y = p4.y/camHeight;
		pts.push_back(p_4);
		
		return pts;
	};
	
	vector <ofPoint> getScaledPoints( int camWidth, int camHeight){
		vector <ofPoint> pts;
		ofPoint p_1;
		p_1.x = (float) (p1.x/(float) camWidth)*(float)ofGetWidth();
		p_1.y = (float) (p1.y/(float) camHeight)*(float)ofGetHeight();
		pts.push_back(p_1);
		
		ofPoint p_2;
		p_2.x = (float) (p2.x/(float) camWidth)*(float)ofGetWidth();
		p_2.y = (float) (p2.y/(float) camHeight)*(float)ofGetHeight();
		pts.push_back(p_2);
		
		ofPoint p_3;
		p_3.x = (float) (p3.x/(float) camWidth)*(float)ofGetWidth();
		p_3.y = (float) (p3.y/(float) camHeight)*(float)ofGetHeight();
		pts.push_back(p_3);
		
		ofPoint p_4;
		p_4.x = (float) (p4.x/(float) camWidth)*(float)ofGetWidth();
		p_4.y = (float) (p4.y/(float) camHeight)*(float)ofGetHeight();
		pts.push_back(p_4);
		
		return pts;
	};
	
};
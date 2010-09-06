/*
 *  Hopscotch.h
 *  Hopscotch Confetti
 *
 *  Created by Brett Renfer on 3/5/10.
 *  Modified by Zack Boka on 8/18/10 to use Optical flow for movement
 *  detection in quads.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

#include "Quad.h"
#include "ofxOsc.h"

//#define NUM_OF_QUADS 9

class CDMblocks: public ofRectangle{
public:
	
	ofxXmlSettings settings;
	int numRects;
	vector <Quad> quads;
//	string names[NUM_OF_QUADS];
	
	ofxTSPSOscSender* sender;
	string oscHost;
	int oscPort;
	
	ofPoint testPoint;
	
	CDMblocks(){
		
//		names[0] = "ONE";
//		names[1] = "TWO";
//		names[2] = "THREE";
//		names[3] = "FOUR";
//		names[4] = "FIVE";
//		names[5] = "SIX";
//		names[6] = "SEVEN";
//		names[7] = "eight";
//		names[8] = "nine";
//		names[9] = "ten";
//		names[10] = "eleven";
//		names[11] = "twelve";
//		names[12] = "thirteen";
//		names[13] = "fourteen";
//		names[14] = "fifteen";
//		names[15] = "sixteen";
//		names[16] = "seventeen";
		
		loadSettings();
		
		x = 350;
		y = 10;
		
//		sender.setup(oscHost, oscPort);
		cout<<"SETTING UP OSC "<<oscHost<<":"<<oscPort<<endl;
	};
	
	void draw(){
		ofPushMatrix();
		ofTranslate(x,y);
		ofNoFill();
		ofCircle(testPoint.x, testPoint.y, 6);
		for (int i=0; i<quads.size(); i++){
			ofSetColor(0xff0000);
			ofBeginShape();
			ofVertex(quads[i].p1.x, quads[i].p1.y);
			ofVertex(quads[i].p2.x, quads[i].p2.y);
			ofVertex(quads[i].p3.x, quads[i].p3.y);
			ofVertex(quads[i].p4.x, quads[i].p4.y);
			ofEndShape(true);			
			
			ofSetColor(255, 0, 0);
			ofEllipse(quads[i].p1.x, quads[i].p1.y, 5, 5);
			ofSetColor(255, 255, 0);
			ofEllipse(quads[i].p2.x, quads[i].p2.y, 5, 5);
			ofSetColor(255, 0, 255);
			ofEllipse(quads[i].p3.x, quads[i].p3.y, 5, 5);
			ofSetColor(0, 255, 0);
			ofEllipse(quads[i].p4.x, quads[i].p4.y, 5, 5);
			
			ofSetColor(0xffffff);
			ofDrawBitmapString("topL", quads[i].p1.x - 15, quads[i].p1.y - 5);
			ofDrawBitmapString("botL", quads[i].p4.x - 15, quads[i].p4.y + 10);
			ofDrawBitmapString(quads[i].name, (quads[i].p1.x + quads[i].p3.x)/2, (quads[i].p1.y + quads[i].p3.y)/2);
		}
		ofFill();
		ofSetColor(0xffffff);
		ofPopMatrix();
	}
	
	Quad testSquare( int x, int y){
		
		testPoint.x = x;
		testPoint.y = y;
		for (int i=0; i<quads.size(); i++){
			// quads[i].active = point_in_convex_quad(x, y, quads[i]);
			float verticesX[4];
			verticesX[0] = quads[i].p1.x;
			verticesX[1] = quads[i].p2.x;
			verticesX[2] = quads[i].p3.x;
			verticesX[3] = quads[i].p4.x;
			
			float verticesY[4];
			verticesY[0] = quads[i].p1.y;
			verticesY[1] = quads[i].p2.y;
			verticesY[2] = quads[i].p3.y;
			verticesY[3] = quads[i].p4.y;
			
			if (pnpoly(4, verticesX, verticesY, x, y)){
				quads[i].active = true;
				return quads[i];
			}
		}
		Quad dummyQuad;
		return dummyQuad;
	}

	
	// ZACK: modified to send the given quad's specific address
	void send(int quadIndex) {
		ofxOscMessage m;
		m.setAddress(quads[quadIndex].getRandomAddress());
		if (DEBUG) cout << "Sending quad: " << m.getAddress() << endl;
		m.addIntArg(quadIndex);
		sender->send(m);
	}

	
	void mousePressed( int mX, int mY ){
		mX -= x;
		mY -= y;
		int hitThreshold = 20;
		int selectedQuad   = -1;
		int selectedPoint  = -1;	
		
		float storeDist = 9999999.0;
		selectedQuad   = -1;
		selectedPoint  = -1;	
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){
			for (int i=0; i<quads.size(); i++){
				Quad q = quads[i];
				q.pressed = false;
				
				for (int j=0; j<4; j++){
					quads[i].selected[j] = false;
				}
				float verticesX[4];
				verticesX[0] = quads[i].p1.x;
				verticesX[1] = quads[i].p2.x;
				verticesX[2] = quads[i].p3.x;
				verticesX[3] = quads[i].p4.x;
				
				float verticesY[4];
				verticesY[0] = quads[i].p1.y;
				verticesY[1] = quads[i].p2.y;
				verticesY[2] = quads[i].p3.y;
				verticesY[3] = quads[i].p4.y;
				
				quads[i].pressed = pnpoly(4, verticesX, verticesY, mX, mY);
				if (quads[i].pressed == true)
					cout << "PRESSED: " << quads[i].name << endl;				
			}
		} else {
			
			for (int i=0; i<quads.size(); i++){
				Quad q = quads[i];
				q.pressed = false;
				
				float dx1 = fabs(mX -  q.p1.x);
				float dy1 = fabs(mY -  q.p1.y);
				float dx2 = fabs(mX -  q.p2.x);
				float dy2 = fabs(mY -  q.p2.y);
				float dx3 = fabs(mX -  q.p3.x);
				float dy3 = fabs(mY -  q.p3.y);
				float dx4 = fabs(mX -  q.p4.x);
				float dy4 = fabs(mY -  q.p4.y);
				
				float dist1 = sqrt(dx1*dx1 + dy1*dy1);
				float dist2 = sqrt(dx2*dx2 + dy2*dy2);
				float dist3 = sqrt(dx3*dx3 + dy3*dy3);
				float dist4 = sqrt(dx4*dx4 + dy4*dy4);
				
				for (int j=0; j<4; j++){
					quads[i].selected[j] = false;
				}
				
				if(dist1 < storeDist && dist1 < hitThreshold){
					selectedQuad = i;
					selectedPoint = 0;
					storeDist = dist1;
				}
				if(dist2 < storeDist && dist2 < hitThreshold){
					selectedQuad = i;
					selectedPoint = 1;
					storeDist = dist2;
				}
				if(dist3 < storeDist && dist3 < hitThreshold){
					selectedQuad = i;
					selectedPoint = 2;
					storeDist = dist3;
				}
				if(dist4 < storeDist && dist4 < hitThreshold){
					selectedQuad = i;
					selectedPoint = 3;
					storeDist = dist4;
				}      
			}
		}
		
		if (selectedQuad != -1 && selectedPoint != -1){
			quads[selectedQuad].selected[selectedPoint] = true;
			cout << "SELECTED: " << quads[selectedQuad].name << endl;
		}
	}
	
	void mouseDragged(int mx, int my){
		for (int i=0; i<quads.size(); i++){
			if (quads[i].pressed){
				quads[i].p2.x = mx - x + fabs(quads[i].p1.x - quads[i].p2.x);		
				quads[i].p2.y = my - y + fabs(quads[i].p1.y - quads[i].p2.y);
				quads[i].p3.x = mx - x + fabs(quads[i].p1.x - quads[i].p3.x);	
				quads[i].p3.y = my - y + fabs(quads[i].p1.y - quads[i].p3.y);	
				quads[i].p4.x = mx - x + fabs(quads[i].p1.x - quads[i].p4.x);		
				quads[i].p4.y = my - y + fabs(quads[i].p1.y - quads[i].p4.y);						
				quads[i].p1.x = mx - x;		
				quads[i].p1.y = my - y;	
				
				cout << "DRAGGED: pressed  " << quads[i].name << endl;
				
			} else if (quads[i].selected[0]){
				quads[i].p1.x = mx - x;		
				quads[i].p1.y = my - y;
				
				cout << "DRAGGED: selected0  " << quads[i].name << endl;
				
			} else if (quads[i].selected[1]){
				quads[i].p2.x = mx - x;		
				quads[i].p2.y = my - y;
				
				cout << "DRAGGED: selected1  " << quads[i].name << endl;
				
			} else if (quads[i].selected[2]){
				quads[i].p3.x = mx - x;		
				quads[i].p3.y = my - y;

				cout << "DRAGGED: selected2  " << quads[i].name << endl;
				
			} else if (quads[i].selected[3]){
				quads[i].p4.x = mx - x;			
				quads[i].p4.y = my - y;	

				cout << "DRAGGED: selected3  " << quads[i].name << endl;			
			
			}
			
			/*for (int j=0; j<4; j++){
				if (quads[i].selected[j]){
					quads[i].points[j]->x = x;
					quads[i].points[j]->y = y;
				}
			}*/
		}
	};
	
	void mouseReleased(){
		for (int i=0; i<quads.size(); i++){
			quads[i].pressed = false;
			for (int j=0; j<4; j++){
				quads[i].selected[j] = false;
			}
		}
		saveSettings();
	};
	
	// ZACK: modified to get the specific quad's image address from the XML file
	void loadSettings(){
		settings.loadFile(ofToDataPath("settings/CDMblockssettings.xml", true));
		
		settings.pushTag("settings",0);
		oscHost = settings.getValue("oscHost", "localhost", 0);
		oscPort = settings.getValue("oscPort", 12000, 0);
		numRects = settings.getNumTags("rect");
		
		for (int i=0; i<numRects; i++){
			settings.pushTag("rect", i);
			Quad q = Quad(settings.getValue("name",""), i);
			cout<<"ADDING QUAD "<<q.name<<","<<i<<endl;

			q.p1.x = settings.getValue("x", 0, 0);
			q.p1.y = settings.getValue("y", 0, 0);
			q.p2.x = settings.getValue("x", 0, 1);
			q.p2.y = settings.getValue("y", 0, 1);
			q.p3.x = settings.getValue("x", 0, 2);
			q.p3.y = settings.getValue("y", 0, 2);
			q.p4.x = settings.getValue("x", 0, 3);
			q.p4.y = settings.getValue("y", 0, 3);
			
			// load in the OSC message addresses
			for (int i = 0; i < settings.getNumTags("messageString"); i++) {
				q.addresses.push_back(settings.getValue("messageString","",i));
			}
			
			settings.popTag();
			quads.push_back(q);
		}		
	}
	
	void saveSettings(){
		settings.loadFile(ofToDataPath("settings/CDMblockssettings.xml", true));
		
		if (!settings.tagExists("settings", 0)){
			settings.addTag("settings");
		}
		
		settings.pushTag("settings",0);
		
		settings.setValue("oscHost", "localhost", 0);
		settings.setValue("oscPort", 12000, 0);
		
		for (int i=0; i<quads.size(); i++){
			if (!settings.tagExists("rect", i)){
				settings.addTag("rect");
			}
			
			settings.pushTag("rect", i);
			Quad q = quads[i];
			settings.setValue("x", q.p1.x,0);
			settings.setValue("y", q.p1.y,0);
			
			settings.setValue("x", q.p2.x,1);
			settings.setValue("y", q.p2.y,1);
			
			settings.setValue("x", q.p3.x,2);
			settings.setValue("y", q.p3.y,2);
			
			settings.setValue("x", q.p4.x,3);
			settings.setValue("y", q.p4.y,3);
			
			settings.popTag();
		}	
		settings.saveFile(ofToDataPath("settings/CDMblockssettings.xml", true));
	}
	
	//UTILS
	
	bool isInsideRect(float x, float y, ofRectangle rect){
		return ( x >= rect.x && x <= rect.x + rect.width && y >= rect.y && y <= rect.y + rect.height );
	}

	bool pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
	{
		bool c = false;
		int i, j;
		for (i = 0, j = nvert-1; i < nvert; j = i++) {
			if ( ((verty[i]>testy) != (verty[j]>testy)) &&
				(testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
				c = !c;
		}
		return c;
	}
	
	int sign(float n){
		if (n > 0) return 1;
		else return -1;
	}
	
	
	// ZACK: return a pointer to the requested quad
	Quad* getQuad(int quad) {
		return &quads[quad];
	}
};
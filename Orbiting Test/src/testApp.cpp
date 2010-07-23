#include "testApp.h"

bool comparisonFunction( particle * a, particle * b ) { 
	return a->mass > b->mass; 
}  

//jg change to test commit

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(0,0,0);
	
	for(int i=0;i<10;i++)
	{
		textures[i].loadImage("square"+ofToString(i)+".png");
	}
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);
	
	
	for (int i = 0; i < 12; i++){
		particle myParticle;
		myParticle.textures = textures;
		myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
	}
	
	//ofSetBackgroundAuto(false);
	
	//char filename[1024];
	//for(int i =0; i < 100; i++){
	//	sprintf(filename, "mypng%5d.png", i);
	//}
	
	int numlines = 200;
	int nodesperline = 20;
	lines.setup(numlines, nodesperline);
	
	ofEnableSmoothing();
	ofSetLogLevel(OF_LOG_SILENT);
	
}

//--------------------------------------------------------------
void testApp::update(){
	for(int v=0;v<vortexes.size();v++)
	{
		vortexes[v].v.resetForce();
		vortexes[v].v.update();
		vortexes[v].v.bounceOffWalls();
		
		for (int j = 0; j < particles.size(); j++){
			particles[j].addRepulsionForce(vortexes[v].v, 60, 1000);
			//particles[i].addCounterClockwiseForce(particles[j], 100, 0.09);
		}
	}
	
	for (int i = 0; i < particles.size(); i++){
		particles[i].resetForce();
		
		for(int v=0;v<vortexes.size();v++)
		{
			particles[i].addAttractionForce(vortexes[v].v.pos.x, vortexes[v].v.pos.y, 200, 4);
			if(vortexes[v].dir==0)
				particles[i].addCounterClockwiseForce(vortexes[v].v,200,.29);
			else
				particles[i].addClockwiseForce(vortexes[v].v,200,.29);
		}
	}
	
	for (int i = 0; i < particles.size(); i++){
		
		//particles[i].addAttractionForce(mouseX, mouseY, 1000, 0.07f);
		
		for (int j = 0; j < i; j++){
			particles[i].addRepulsionForce(particles[j], 20, 4);
			//particles[i].addCounterClockwiseForce(particles[j], 100, 0.09);
		}
	}
	
	for (int i = 0; i < particles.size(); i++){
		particles[i].addDampingForce();
		particles[i].update();
		particles[i].bounceOffWalls();
	}
	
	
	
	//old mass stuff
	/*sort( objects.begin(), objects.end(), comparisonFunction );
	
	for(int i=objects.size()-1;i>=0;i--)
	{
		if(i->mass<=1) //if we're down to the smallest particles, break!
			break;
		
		for(int o=i-1;i>=0;i--)
		{
			if(objects[o]->mass < objects[i]->mass)//affect it
			{
				objects[o]->addForce(objects[i
			}
		}
	}*/
	
	lines.update(particles);
}

//--------------------------------------------------------------
void testApp::draw(){
	ofFill();
	ofSetRectMode(OF_RECTMODE_CORNER);
	ofEnableAlphaBlending();
	
	ofPushStyle();
	
	lines.draw();
	
	ofPopStyle();
	
	ofPushStyle();
	
	ofSetColor(0,130,130, 200);
	
	ofSetColor(0x000000);
	ofNoFill();
	for (int i = 0; i < particles.size(); i++){
		ofSetColor(0,0,0);
		particles[i].draw();
	}
	
	ofSetColor(0xff0000);
	for(int v=0;v<vortexes.size();v++)
	{
		ofFill();
		ofSetColor(255,0,0);
		vortexes[v].v.draw();
	}
	
	
	ofPopStyle();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	vort V;
	V.v.setInitialCondition(x, y, ofRandomf(), ofRandomf());
	V.dir=button;
	
	vortexes.push_back(V);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}


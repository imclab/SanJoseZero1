/*
 *  LineGenerator.cpp
 *  orbitalExperiments
 *
 *  Created by Jim on 7/23/10.
 *  Copyright 2010 University of Washington. All rights reserved.
 *
 */

#include "LineGenerator.h"

#define MAX_WIDTH 9
#define MIN_WIDTH 8

#define MAX_DISTANCE 100


void LineGenerator::setup(int numLines, int numNodes)
{	
	large.loadImage("large_texture.png");
	
	float lineDistance = 1.0*ofGetWidth() / numLines;
	float nodeDistance = 1.0*ofGetHeight() / numNodes;
	for(int l = 0; l < numLines; l++){
		for(int n = 0; n < numNodes; n++){
			LineNode node;
			node.pos = node.original = ofxPoint2f(l*lineDistance, n*nodeDistance);
			
			node.width = MIN_WIDTH;
			if(n != 0){
				node.prev = &(nodes[n-1]);
			}
			else{
				node.prev = NULL;
			}
			nodes.push_back(node);
		}
	}
	
	mask.allocate(ofGetWidth(), ofGetHeight(), GL_RGB, 4);
	
	shader.loadShader("ShardComposite");
	shader.setShaderActive(true);
	shader.setUniform("Tex0", 0);
	shader.setUniform("Tex1", 1);
	shader.setShaderActive(false);	
}

void LineGenerator::update(vector<particle>& points)
{
	for(int p = 0; p < points.size(); p++){
		particle& point = points[p];
		for(int n = 0; n < nodes.size(); n++){
			LineNode& node = nodes[n];
			ofxVec2f direction = (point.pos - node.pos);
			float distance = direction.length();
			if(distance < MAX_DISTANCE){
				float angle = direction.angle(point.vel);
				if(angle < 180 && angle > -180){
					node.pos -= direction/50.0 * point.vel.length();
				}
				else{
					node.pos += direction/100.0 * point.vel.length();
				}
				float addwidth = (MAX_DISTANCE - distance)/100.0 * point.vel.length();
				node.width = MIN(node.width + addwidth, MAX_WIDTH);
			}
		}
	}
	
	//shrink lines.
	for(int i = 0; i < nodes.size(); i++){
		nodes[i].width = MAX(nodes[i].width - .3, MIN_WIDTH);
		nodes[i].pos += (nodes[i].original - nodes[i].pos)*.1;
	}
}

void LineGenerator::draw()
{
	mask.clear(0, 0, 0, 0);
	mask.begin();
	
	ofEnableAlphaBlending();
	
	for(int i = 0; i < nodes.size(); i++){
		LineNode n = nodes[i];
		if(n.prev != NULL){
			spline.push_back(nodes[i].pos);
		}
		else {
			glBegin(GL_TRIANGLE_STRIP);
			for(float p = 0; p <= 1.0; p += .005){
				ofxPoint2f point = spline.sampleAt(p);
				glColor4f(1.0, 1.0, 1.0, ofMap(n.width, MIN_WIDTH, MAX_WIDTH, 0.2, .8));
//				glVertex2f(n.pos.x - n.width/2, n.pos.y);
//				glVertex2f(n.pos.x + n.width/2, n.pos.y);
				glVertex2f(point.x - 1, point.y);
				glVertex2f(point.x + 1, point.y);
			}
			glEnd();
			spline.clear();
		}
	}
	
	mask.end();
	
	ofDisableAlphaBlending();
	
	glActiveTexture(GL_TEXTURE0_ARB);
	large.getTextureReference().bind();
	
	glActiveTexture(GL_TEXTURE1_ARB);
	mask.bind();
	
	shader.setShaderActive(true);
	
	glBegin(GL_QUADS);
	
	glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, 0);
	glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, 0);
	glVertex2f(0, ofGetHeight());
	
	glMultiTexCoord2f(GL_TEXTURE0_ARB, large.getWidth(), 0);
	glMultiTexCoord2f(GL_TEXTURE1_ARB, mask.getWidth(), 0);		
	glVertex2f( ofGetWidth(), ofGetHeight() );
	
	glMultiTexCoord2f(GL_TEXTURE0_ARB, large.getWidth(), large.getHeight());
	glMultiTexCoord2f(GL_TEXTURE1_ARB, mask.getWidth(), mask.getHeight());
	glVertex2f( ofGetWidth(), 0);
	
	glMultiTexCoord2f(GL_TEXTURE0_ARB, 0, large.getHeight());
	glMultiTexCoord2f(GL_TEXTURE1_ARB, 0, mask.getHeight());
	glVertex2f(0, 0);
	
	glEnd();
	
	shader.setShaderActive(false);
	
	glActiveTexture(GL_TEXTURE1_ARB);
	mask.unbind();
	
	glActiveTexture(GL_TEXTURE0_ARB);
	large.getTextureReference().unbind();
	
	//mask.draw(0, 0);
	
	ofEnableAlphaBlending();
	
}

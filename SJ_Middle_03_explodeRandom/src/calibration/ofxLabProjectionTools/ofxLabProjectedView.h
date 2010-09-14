/*
 *  ofxLabProjectedView.h
 *
 *  Created by Brett Renfer on 3/16/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */
/*
 
 Based in part off of ofxTouchGraphicsWarp:
 
 * Copyright 2008 NOR_/D <http://nortd.com>
 *
 * This file is part of ofxTouch.
 * ofxTouch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation, either version 3 of 
 * the License, or (at your option) any later version. For details
 * see <http://www.gnu.org/licenses/>.
 *
 * * *
 * This class handles perspective warping of the entire OpenGL world.
 *
 */

#pragma once
#include "ofxFBOTexture.h"
//#include "ofxFBOTexture.h"
#include "ofxLabProjectionOverlay.h"
#include "ofxLabUtils.h"


class ofxLabProjectedView
{
public:
	
	int index;
	ofPoint skewCorners[4];
	float skewCornerVertices[8];
	bool selected[4];
	int selectedPoint;
	
	ofPoint cropCorners[4];
	float cropCornerVertices[8];
	bool bCropSelected[4];
	int selectedCropPoint;
	
	float texCoords[8];
	float cropTexCoords[8];
	
	//int selected;
	bool bDrawSkew;
	bool bDrawCrop;
	bool bMovable;
	bool bMoving;
	ofPoint pressed;
	
	//overlay drawing
	bool bDrawOverlay;
	bool bNewOverlay, bOverlaySelected;
	ofxLabProjectionOverlay * currentOverlay;
	
	ofRectangle resolution;
	
	float x, y, width, height;
	ofxFBOTexture texture;
	
	ofxLabProjectionOverlay edges[4];
	vector <ofxLabProjectionOverlay *> overlays;
	
	float minX, maxX, minY, maxY;
	
//----------------------------------------------------------------------------
	
	ofxLabProjectedView( int _index, float _x, float _y, float _width, float _height):
	index(_index),
	x(_x), 
	y(_y),
	width(_width), 
	height(_height)
	{		
		skewCorners[0].x = cropCorners[0].x = x;
		skewCorners[0].y = cropCorners[0].y = y;
		skewCorners[1].x = cropCorners[1].x = x + width;
		skewCorners[1].y = cropCorners[1].y = y;
		skewCorners[2].x = cropCorners[2].x = x + width;
		skewCorners[2].y = cropCorners[2].y = y + height;
		skewCorners[3].x = cropCorners[3].x = x;
		skewCorners[3].y = cropCorners[3].y = y + height;
		
		skewCornerVertices[0] = cropCornerVertices[0] =skewCorners[0].x;
		skewCornerVertices[1] = cropCornerVertices[1] =skewCorners[0].y;
		skewCornerVertices[2] = cropCornerVertices[2] =skewCorners[1].x;
		skewCornerVertices[3] = cropCornerVertices[3] =skewCorners[1].y;
		skewCornerVertices[4] = cropCornerVertices[4] =skewCorners[2].x;
		skewCornerVertices[5] = cropCornerVertices[5] =skewCorners[2].y;
		skewCornerVertices[6] = cropCornerVertices[6] =skewCorners[3].x;
		skewCornerVertices[7] = cropCornerVertices[7] =skewCorners[3].y;
		
		texture.allocate(width, height, GL_RGBA, 4);
		texture.clear();
		
		bDrawSkew = false;
		bDrawCrop = false;
		
		//skew
		selectedPoint  = -1;
		for (int j=0; j<4; j++){
			selected[j] = false;
		}
		
		//crop
		selectedCropPoint = -1;
		for (int j=0; j<4; j++){
			bCropSelected[j] = false;
		}
		
		//edge overlays
		for (int i=0; i<4; i++){
			edges[i].loadImage("overlay/overlay.png", width, height);
			edges[i].width = width;		
			edges[i].height = 0;
		}
		
		edges[0].x = width;
		edges[0].y = height;
		edges[0].width = width;		
		edges[0].height = 0;
		edges[0].rotation = 180;
		
		edges[1].x = width;
		edges[1].y = 0;
		edges[1].width = height;	
		edges[1].rotation = 90;
		
		edges[2].x = 0;
		edges[2].y = 0;
		edges[2].rotation = 0;
		
		edges[3].x = 0;
		edges[3].y = height;
		edges[3].width = height;
		edges[3].rotation = 270;
		
		//overlays
		bNewOverlay = bOverlaySelected = bDrawOverlay = false;
		bMovable = false;
		bMoving = false;
	};
	
	void reset(){		
		skewCorners[0].x = cropCorners[0].x = x;
		skewCorners[0].y = cropCorners[0].y = y;
		skewCorners[1].x = cropCorners[1].x = x + width;
		skewCorners[1].y = cropCorners[1].y = y;
		skewCorners[2].x = cropCorners[2].x = x + width;
		skewCorners[2].y = cropCorners[2].y = y + height;
		skewCorners[3].x = cropCorners[3].x = x;
		skewCorners[3].y = cropCorners[3].y = y + height;
		
		skewCornerVertices[0] = cropCornerVertices[0] =skewCorners[0].x;
		skewCornerVertices[1] = cropCornerVertices[1] =skewCorners[0].y;
		skewCornerVertices[2] = cropCornerVertices[2] =skewCorners[1].x;
		skewCornerVertices[3] = cropCornerVertices[3] =skewCorners[1].y;
		skewCornerVertices[4] = cropCornerVertices[4] =skewCorners[2].x;
		skewCornerVertices[5] = cropCornerVertices[5] =skewCorners[2].y;
		skewCornerVertices[6] = cropCornerVertices[6] =skewCorners[3].x;
		skewCornerVertices[7] = cropCornerVertices[7] =skewCorners[3].y;
	}
	
//----------------------------------------------------------------------------
	
	void drawDebug(){
		glDisable(GL_DEPTH_TEST);
		for (int i=0; i<overlays.size(); i++){
			overlays[i]->render();
			overlays[i]->draw();
		};
		
		float squareWidth = 10;
		if (bDrawSkew){
			ofPushStyle();{
				ofSetColor(255,0,0);
				ofRect(skewCorners[0].x, skewCorners[0].y, squareWidth, squareWidth);
				ofSetColor(0,255,0);
				ofRect(skewCorners[1].x-squareWidth, skewCorners[1].y, squareWidth, squareWidth);
				ofSetColor(0,0,255);
				ofRect(skewCorners[2].x-squareWidth, skewCorners[2].y-squareWidth, squareWidth, squareWidth);
				ofSetColor(255,255,0);
				ofRect(skewCorners[3].x, skewCorners[3].y-squareWidth, squareWidth, squareWidth);
			}ofPopStyle();
		} else if (bDrawCrop){ 
			ofSetColor(0xffffff);
			
			ofPushStyle();{
				ofSetColor(150,50,150);
				ofRect(cropCorners[0].x, cropCorners[0].y, squareWidth, squareWidth);
				ofSetColor(0,150,150);
				ofRect(cropCorners[1].x-squareWidth, cropCorners[1].y, squareWidth, squareWidth);
				ofSetColor(150,100,0);
				ofRect(cropCorners[2].x-squareWidth, cropCorners[2].y-squareWidth, squareWidth, squareWidth);
				ofSetColor(50,150,50);
				ofRect(cropCorners[3].x, cropCorners[3].y-squareWidth, squareWidth, squareWidth);
			}ofPopStyle();			
		} 
		
		ofSetColor(0x000000);
		glDisable(GL_DEPTH_TEST);
		ofQuad3D(cropCorners[1],ofPoint(width,0),ofPoint(width,height),cropCorners[2]);
		ofQuad3D(ofPoint(0,0),cropCorners[0],cropCorners[3],ofPoint(0,height));
		ofSetColor(0xffffff);
			
	};
	
	void draw(){
		
		ofEnableAlphaBlending();
		//if (!bDrawCrop){
			if (!bMoving) texture.bind();
			else ofSetColor(255,255,0);
			
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, &cropTexCoords[0]);
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, &skewCornerVertices[0]);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			
			/*
			glBegin(GL_QUADS);{		
				glTexCoord2f(fmax(0,cropCorners[0].x-x),fmax(0,cropCorners[0].y-y));
				glVertex2f(skewCorners[0].x, skewCorners[0].y);		
				
				glTexCoord2f(fmin(width,cropCorners[1].x-x),fmax(0,cropCorners[1].y-y));
				glVertex2f(skewCorners[1].x, skewCorners[1].y);			
				
				glTexCoord2f(fmin(width,cropCorners[2].x-x),fmin(height,cropCorners[2].y-y));
				glVertex2f(skewCorners[2].x, skewCorners[2].y);			
				
				glTexCoord2f(fmax(0,cropCorners[3].x-x),fmin(height,cropCorners[3].y-y));
				glVertex2f(skewCorners[3].x, skewCorners[3].y);	
			}glEnd(); 
			 */
			
			texture.unbind();
			ofSetColor(0xffffff);
			
			ofEnableAlphaBlending();
			
			glDisable(GL_DEPTH_TEST);
			
			//render overlay fbos
			
			for (int i=0; i<4; i++){		
				edges[i].bind();
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(2, GL_FLOAT, 0, &texCoords[0]);
				
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(2, GL_FLOAT, 0, &skewCornerVertices[0]);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				/*
				glBegin(GL_QUADS);{
					glTexCoord2f(0,0);
					glVertex2f(skewCorners[0].x, skewCorners[0].y);		
					glTexCoord2f(width,0);
					glVertex2f(skewCorners[1].x, skewCorners[1].y);		
					glTexCoord2f(width,height);
					glVertex2f(skewCorners[2].x, skewCorners[2].y);		
					glTexCoord2f(0,height);
					glVertex2f(skewCorners[3].x, skewCorners[3].y);
				}glEnd(); 
				 */
				edges[i].unbind();
			}		
			
			
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			
			for (int i=0; i<overlays.size(); i++){
				overlays[i]->render();
				overlays[i]->draw();
			};
		//}
			
		float squareWidth = 10;
		
		if (bDrawSkew){
			ofPushStyle();{
				ofSetColor(255,0,0);
				ofRect(skewCorners[0].x, skewCorners[0].y, squareWidth, squareWidth);
				ofSetColor(0,255,0);
				ofRect(skewCorners[1].x-squareWidth, skewCorners[1].y, squareWidth, squareWidth);
				ofSetColor(0,0,255);
				ofRect(skewCorners[2].x-squareWidth, skewCorners[2].y-squareWidth, squareWidth, squareWidth);
				ofSetColor(255,255,0);
				ofRect(skewCorners[3].x, skewCorners[3].y-squareWidth, squareWidth, squareWidth);
			}ofPopStyle();
		} else if (bDrawCrop){ 
			ofEnableAlphaBlending();
			ofSetColor(255,255,255,150);
			ofPushMatrix();
			ofScale(1, -1, 1);
			ofTranslate(0, -height);
			texture.draw(x, y);
			ofPopMatrix();
			ofDisableAlphaBlending();
			ofSetColor(0xffffff);
			
			ofPushStyle();{
				ofSetColor(150,50,150);
				ofRect(cropCorners[0].x, cropCorners[0].y, squareWidth, squareWidth);
				ofSetColor(0,150,150);
				ofRect(cropCorners[1].x-squareWidth, cropCorners[1].y, squareWidth, squareWidth);
				ofSetColor(150,100,0);
				ofRect(cropCorners[2].x-squareWidth, cropCorners[2].y-squareWidth, squareWidth, squareWidth);
				ofSetColor(50,150,50);
				ofRect(cropCorners[3].x, cropCorners[3].y-squareWidth, squareWidth, squareWidth);
			}ofPopStyle();			
		};
	};
	
	
//----------------------------------------------------------------------------
	
	void beginDraw(){
		texture.begin();
		texture.clear();
		
		ofPushMatrix();
		ofScale(1,-1);
		ofTranslate(0,-height);
		//ofTranslate(-x, -y);
	}
	
//----------------------------------------------------------------------------
	
	void endDraw(){
		ofPopMatrix();
		texture.end();
		
		for (int i=0; i<4; i++){
			edges[i].render();			
		}	
	}
	
//----------------------------------------------------------------------------
	
	ofxFBOTexture getTexture(){
		return texture;
	}
//----------------------------------------------------------------------------
	ofPoint getResolution(){
		return ofPoint(width,height);
	} 
	
//----------------------------------------------------------------------------
	ofPoint getRenderPosition(){
		return ofPoint(x,y);
	}
	
//----------------------------------------------------------------------------
	float getCroppedWidth(){
		return (cropCorners[1].x - cropCorners[0].x);
	}
					   
					   
//----------------------------------------------------------------------------
	
	void drawSkew( int skew=true ){
		bDrawSkew = skew;
		if (bDrawSkew) bDrawCrop =false;
	}

//----------------------------------------------------------------------------
	
	void setSkewPoint( ofPoint skewValue, int which ){
		skewCorners[which].x = skewValue.x;
		skewCorners[which].y= skewValue.y;
		resetPoints();
	}
	
	
//----------------------------------------------------------------------------
	void resetPoints(){
		minX = 9999;
		maxX = -9999;
		minY = 9999;
		maxY = -9999;
		
		for (int i=0; i<4; i++){
			if (skewCorners[i].x < minX) minX = skewCorners[i].x;
			else if (skewCorners[i].x > maxX) maxX = skewCorners[i].x;
			if (skewCorners[i].y < minY) minY = skewCorners[i].y;
			else if (skewCorners[i].y > maxY) maxY = skewCorners[i].y;					
		}
		
		cropTexCoords[0] = fmax(0,cropCorners[0].x-x);
		cropTexCoords[1] = fmax(0,cropCorners[0].y-y);
		cropTexCoords[2] = fmin(width,cropCorners[1].x-x);
		cropTexCoords[3] = fmax(0,cropCorners[1].y-y);
		cropTexCoords[4] = fmin(width,cropCorners[2].x-x);
		cropTexCoords[5] = fmin(height,cropCorners[2].y-y);
		cropTexCoords[6] = fmax(0,cropCorners[3].x-x);
		cropTexCoords[7] = fmin(height,cropCorners[3].y-y);
		
		texCoords[0] = 0;
		texCoords[1] = 0;
		texCoords[2] = width;
		texCoords[3] = 0;
		texCoords[4] = width;
		texCoords[5] = height;
		texCoords[6] = 0;
		texCoords[7] = height;
				
		skewCornerVertices[0] = skewCorners[0].x;
		skewCornerVertices[1] = skewCorners[0].y;
		skewCornerVertices[2] = skewCorners[1].x;
		skewCornerVertices[3] = skewCorners[1].y;
		skewCornerVertices[4] = skewCorners[2].x;
		skewCornerVertices[5] = skewCorners[2].y;
		skewCornerVertices[6] = skewCorners[3].x;
		skewCornerVertices[7] = skewCorners[3].y;
		
		cropCornerVertices[0] = cropCorners[0].x;
		cropCornerVertices[1] = cropCorners[0].y;
		cropCornerVertices[2] = cropCorners[1].x;
		cropCornerVertices[3] = cropCorners[1].y;
		cropCornerVertices[4] = cropCorners[2].x;
		cropCornerVertices[5] = cropCorners[2].y;
		cropCornerVertices[6] = cropCorners[3].x;
		cropCornerVertices[7] = cropCorners[3].y;
	}
	
//----------------------------------------------------------------------------
	
	ofPoint getSkewPoint( int which ){
		return skewCorners[which];
	}
	
//----------------------------------------------------------------------------
	
	void drawCrop( bool crop=true ){
		bDrawCrop = crop;
		if (bDrawCrop) bDrawSkew =false;
	}	
	
//----------------------------------------------------------------------------
	
	void setCropPoint( ofPoint cropValue, int which ){
		cropCorners[which].x = cropValue.x;
		cropCorners[which].y= cropValue.y;
		
		resetPoints();
	}	
	
//----------------------------------------------------------------------------
	
	ofPoint getCropPoint( int which ){
		return cropCorners[which];
	}
	
//----------------------------------------------------------------------------
	
	void drawOverlay( bool overlay=true ){
		bDrawOverlay = overlay;
		if (bDrawOverlay){
			bDrawSkew =false;
			bDrawCrop = false;
		} else {
			bNewOverlay = false;
		}
	}
	
//----------------------------------------------------------------------------
	void addOverlay( ofxLabProjectionOverlay * overlay){
		overlays.push_back(overlay);
	}
	
//----------------------------------------------------------------------------
	ofxLabProjectionOverlay * addOverlay(){
		ofxLabProjectionOverlay * overlay = new ofxLabProjectionOverlay();
		overlays.push_back(overlay);
		return overlay;
	}
	
//----------------------------------------------------------------------------

	bool mousePressed( int mx, int my ){
		int hitThreshold = 10;
		selectedPoint  = -1;	
		float storeDist = 9999999.0;
		
		for (int j=0; j<4; j++){
			selected[j] = false;
			bCropSelected[j] = false;
		}
		
		if (bDrawSkew){
			if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){
			} else {			
				float dx1 = fabs(mx -  skewCorners[0].x);
				float dy1 = fabs(my -  skewCorners[0].y);
				float dx2 = fabs(mx -  skewCorners[1].x);
				float dy2 = fabs(my -  skewCorners[1].y);
				float dx3 = fabs(mx -  skewCorners[2].x);
				float dy3 = fabs(my -  skewCorners[2].y);
				float dx4 = fabs(mx -  skewCorners[3].x);
				float dy4 = fabs(my -  skewCorners[3].y);
				
				float dist1 = sqrt(dx1*dx1 + dy1*dy1);
				float dist2 = sqrt(dx2*dx2 + dy2*dy2);
				float dist3 = sqrt(dx3*dx3 + dy3*dy3);
				float dist4 = sqrt(dx4*dx4 + dy4*dy4);
				
				if(dist1 < storeDist && dist1 < hitThreshold){
					selectedPoint = 0;
					storeDist = dist1;
				}
				
				if(dist2 < storeDist && dist2 < hitThreshold){
					selectedPoint = 1;
					storeDist = dist2;
				}
				
				if(dist3 < storeDist && dist3 < hitThreshold){
					selectedPoint = 2;
					storeDist = dist3;
				}
				
				if(dist4 < storeDist && dist4 < hitThreshold){
					selectedPoint = 3;
					storeDist = dist4;
				}      
				
			}
			
			if (selectedPoint != -1){
				selected[selectedPoint] = true;
				return true;
			}
			return false;
		} else if (bDrawCrop){
			if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){
			} else {			
				float dx1 = fabs(mx -  cropCorners[0].x);
				float dy1 = fabs(my -  cropCorners[0].y);
				float dx2 = fabs(mx -  cropCorners[1].x);
				float dy2 = fabs(my -  cropCorners[1].y);
				float dx3 = fabs(mx -  cropCorners[2].x);
				float dy3 = fabs(my -  cropCorners[2].y);
				float dx4 = fabs(mx -  cropCorners[3].x);
				float dy4 = fabs(my -  cropCorners[3].y);
				
				float dist1 = sqrt(dx1*dx1 + dy1*dy1);
				float dist2 = sqrt(dx2*dx2 + dy2*dy2);
				float dist3 = sqrt(dx3*dx3 + dy3*dy3);
				float dist4 = sqrt(dx4*dx4 + dy4*dy4);
				
				if(dist1 < storeDist && dist1 < hitThreshold){
					selectedCropPoint = 0;
					storeDist = dist1;
				}
				
				if(dist2 < storeDist && dist2 < hitThreshold){
					selectedCropPoint = 1;
					storeDist = dist2;
				}
				
				if(dist3 < storeDist && dist3 < hitThreshold){
					selectedCropPoint = 2;
					storeDist = dist3;
				}
				
				if(dist4 < storeDist && dist4 < hitThreshold){
					selectedCropPoint = 3;
					storeDist = dist4;
				}      
				
			}
			
			if (selectedCropPoint != -1){
				bCropSelected[selectedCropPoint] = true;
				return true;
			}
			return false;
		} else if (bDrawOverlay){
			bool bDrawable = ((mx > minX) && (mx < maxX) && (my > minY) && (my < maxY));
			if (bDrawable){
				if (!bNewOverlay){
					//check other overlays for click
					bOverlaySelected = false;
					
					for (int i=0; i<overlays.size(); i++){
						if (overlays[i]->hitTest(mx, my)){
							bNewOverlay = false;
							currentOverlay = overlays[i];						
							currentOverlay->resetPositioning(mx, my);						
							bOverlaySelected = true;
							return true;
						}
					};
					
					//no? add new
					if (!bOverlaySelected){
						bNewOverlay = true;
						currentOverlay = addOverlay();
						currentOverlay->setPosition(mx,my);
						currentOverlay->addPoint(0,0); 
						currentOverlay->bSelected = true;
					}
				} else {				
					currentOverlay->addPoint(mx-currentOverlay->pos.x,my-currentOverlay->pos.y);
				}
			} 
			return bDrawable;
		} else if (bMovable){			
			bMoving = ((mx > minX) && (mx < maxX) && (my > minY) && (my < maxY));
			pressed.x = mx;
			pressed.y = my;
			return bMoving;
		};
	}
	
	
//----------------------------------------------------------------------------
	
	void mouseDragged( int mx, int my ){
		if (bDrawSkew){
			if (selectedPoint == -1) return;
			
			for (int i=0; i<4; i++){
				if (selected[i]){
					skewCorners[i].x = fmax(0,fmin(mx, ofGetWidth()));
					skewCorners[i].y = fmax(0,fmin(my, ofGetHeight()));
				}
			}
			
			resetPoints();
			
		} else if (bDrawCrop){
			if (selectedCropPoint == -1) return;			
			for (int i=0; i<4; i++){
				if (bCropSelected[i]){
					cropCorners[i].x = fmax(x,fmin(mx, x+width));
					cropCorners[i].y = fmax(y,fmin(my, y+height));
					skewCorners[i].x = fmax(x,fmin(mx, x+width));
					skewCorners[i].y = fmax(y,fmin(my, y+height));
				}
			}
			resetPoints();
		} else if (bDrawOverlay){
			if (bOverlaySelected){				
				currentOverlay->setPosition(mx,my);				
				//currentOverlay->resetPositioning(mx, my);
			};
		} else if (bMovable && bMoving){
			for (int i=0; i<4; i++){
				skewCorners[i].x -= pressed.x - mx;
				skewCorners[i].y -= pressed.y -my;	
				//cropCorners[i].x += pressed.x -mx;	
				//cropCorners[i].y += pressed.y -mx;
			}
			pressed.x = mx;
			pressed.y = my;
		};
	}
	
//----------------------------------------------------------------------------
	
	void mouseReleased(){	
		selectedCropPoint = selectedPoint = -1;
		for (int j=0; j<4; j++){
			selected[j] = false;
			bCropSelected[j] = false;
		}
		
		for (int i=0; i<overlays.size(); i++){
			if (!bNewOverlay)
				overlays[i]->bSelected = false;
		}
		
		bOverlaySelected = false;
		bMoving = false;
	}	
	
//----------------------------------------------------------------------------
	
	void keyPressed (int key){
		if (key == OF_KEY_BACKSPACE){
			if (bOverlaySelected || bNewOverlay){
				
				bOverlaySelected = bNewOverlay = false;
				for (int i=overlays.size()-1; i>=0; i--){
					if (overlays[i] == currentOverlay){
						overlays.erase(overlays.begin()+i);
					};
				}
			}
		} else if (key == OF_KEY_RETURN){
			if (bNewOverlay){
				bNewOverlay = false;
				currentOverlay->setPosition(currentOverlay->pos.x,currentOverlay->pos.y);
				for (int i=0; i<overlays.size(); i++){
					overlays[i]->bSelected = false;
				}
			}
		};
	};	
};


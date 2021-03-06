/*
 *  guiTypeMultiSelectToggle.h
 *  CosmoSystem
 *
 *  Created by Brett Renfer on 4/29/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once


#include "guiBaseObject.h"
#include "guiColor.h"
#include "simpleColor.h"
#include "guiValue.h"

#define boxSpacing		2
#define boxSize			14

class guiTypeMultiSelectToggle : public guiBaseObject{
	
public:
	
	//------------------------------------------------
	void updateBoundingBox(){
		hitArea.y = boundingBox.y;
		boundingBox.height = 0;
		
		for(int i = 0; i < bNames .size(); i++){
			boundingBox.height += boxSize+boxSpacing;
		}
		hitArea.height = boundingBox.height;
		
	}
	
	void setFont(ofTrueTypeFont * fontPtr){
		displayText.setFont(fontPtr);
	}
	
	//------------------------------------------------
	void setup(string multiToggleName, vector <bool> defaultSelected, vector <string> boxNames){
		bNames = boxNames;
		for (int i=0; i<defaultSelected.size(); i++){
			value.addValueB( (bool)defaultSelected[i]);
		}
		
		name = multiToggleName;
		
		float lineHeight = 0;
		for(int i = 0; i < bNames .size(); i++){
			float lineWidth = boxSpacing + boxSize + displayText.stringWidth(bNames[i]);
			
			if( lineWidth > hitArea.width ){
				hitArea.width       += lineWidth-hitArea.width;
				boundingBox.width   += lineWidth-hitArea.width;
			}
			
			lineHeight += displayText.stringHeight(bNames[i])+boxSpacing;
		}
		
		if(lineHeight > hitArea.height){
			hitArea.height      += lineHeight-hitArea.height;
			boundingBox.height  += lineHeight-hitArea.height;
		}
		setTypeBool();
		dataType = SG_TYPE_BOOL;		
	}
	
	//-----------------------------------------------.
	bool updateGui(float x, float y, bool firstHit, bool isRelative){
		if(!firstHit)return false;
		
		if( state == SG_STATE_SELECTED){
			float relX = x - hitArea.x;
			float relY = y - hitArea.y;
			
			for(int i = 0; i < bNames.size(); i++){
				ofRectangle tmpRect(0, i * (boxSize+boxSpacing), boxSize, boxSize);
				if( isInsideRect(relX, relY, tmpRect) ){
					if (value.getValueB(i)) value.setValue(0, i);
					else value.setValue(1, i);
					//break;
				}
			}
			return true;
		}
		return false;		
	}
	
	//-----------------------------------------------.
	void render(){
		ofPushStyle();
		glPushMatrix();
		guiBaseObject::renderText(hitArea.x + hitArea.width + borderWidth, hitArea.y - 3);
		
		//draw the background
		ofFill();
		glColor4fv(bgColor.getColorF());
		ofRect(hitArea.x, hitArea.y, hitArea.width, hitArea.height);
		
		for(int i = 0; i < bNames.size(); i++){
			float bx = hitArea.x + 0;
			float by = hitArea.y + i * (boxSize+boxSpacing);
			
			if(value.getValueB(i)){
				ofFill();
			}else{
				ofNoFill();
			}
			
			glColor4fv(fgColor.getColorF());
			ofRect(bx, by, boxSize, boxSize);
			
			ofNoFill();
			glColor4fv(outlineColor.getColorF());
			ofRect(bx, by, boxSize, boxSize);
			
			glColor4fv(textColor.getColorF());
			displayText.renderString(bNames[i], bx + boxSize + boxSpacing, by + boxSize -2);
		}
		
		glPopMatrix();
		ofPopStyle();
	}
	
    vector <string> bNames;
};

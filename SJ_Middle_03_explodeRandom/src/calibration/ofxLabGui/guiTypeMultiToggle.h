#pragma once


#include "guiBaseObject.h"
#include "guiColor.h"
#include "simpleColor.h"
#include "guiValue.h"

#define boxSpacing		2
#define boxSize			14

class guiTypeMultiToggle : public guiBaseObject{

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
        void setup(string multiToggleName, int defaultBox, vector <string> boxNames){
            bNames = boxNames;
            value.addValue( (int)defaultBox, 0, bNames.size()-1);
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
                        value.setValue(i, 0);
                        break;
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

                    if(value.getValueI() == i){
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

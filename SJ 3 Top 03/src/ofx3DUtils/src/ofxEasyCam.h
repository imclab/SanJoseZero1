/*
 In your test class, add a member variable:
  ofxEasyCam camera;

 Finally, in your draw(), say:
  camera.place();
 To render from the camera's perspective. To draw a HUD, add:
  camera.remove();
 When you're done drawing from the camera's perspective.
*/

/*
 A better implementation would use quaternions,
 code for which is described at the end of this presentation:

 http://www.cs.sunysb.edu/~mueller/teaching/cse564/trackball.ppt

 The code has the advantage of not requiring an implementation of
 a quaternion class.
*/

#pragma once

#include "ofx3DUtils.h"
#include "ofxXmlSettings.h"

class ofxEasyCam : public ofxCamera {
private:
	float startMomentum, stopMomentum;
	bool mouseHasMoved, mouseClicked;
	float dmouseX, dmouseY, pmouseX, pmouseY;
	int mouseButton;
	bool bShiftDown;
	bool bIsEnabled;
	ofxXmlSettings settings;
		
public:
	float zoomSpeed, orbitSpeed, panSpeed;

	ofxEasyCam() :
		startMomentum(.5),
		stopMomentum(.5),
		mouseHasMoved(false),
		mouseClicked(false),
		dmouseX(0), dmouseY(0),
		pmouseX(0), pmouseY(0),
		zoomSpeed(5),
		orbitSpeed(.2),
		panSpeed(1)
		{

		ofAddListener(ofEvents.mousePressed, this, &ofxEasyCam::mousePressed);
		ofAddListener(ofEvents.mouseReleased, this, &ofxEasyCam::mouseReleased);
		ofAddListener(ofEvents.mouseDragged, this, &ofxEasyCam::mouseDragged);

		float theta = PI * fieldOfView / 360.0f;
		float opposite = ofGetWidth() / 2;
		posCoord.z = opposite / tanf(theta);
		bShiftDown = false;
		bIsEnabled = false;
			
		loadSettings();
	}
	void place() {
		ofxVec3f relx = getDir().getCrossed(getUp()).normalize();
		ofxVec3f rely = getDir().getCrossed(relx).normalize();
				
		if(mouseButton == 1 || bShiftDown) {
			ofxVec3f offset =
			(relx * dmouseX * panSpeed) +
			(rely * dmouseY * panSpeed);
			moveGlobal(offset);
		} else if(mouseButton == 0) {
			ofxVec3f rot = relx * dmouseY + rely * -dmouseX;
			orbitAround(getEye(), rot, rot.length() * orbitSpeed);
		}  else if(mouseButton == 2) {
			ofxVec3f offset = getDir().normalize() * (zoomSpeed * dmouseY);
			eye(getEye() - offset);
			moveGlobal(offset);
		}
		dmouseX *= stopMomentum;
		dmouseY *= stopMomentum;
		ofxCamera::place();
		saveSettings();
	}
	
	void mousePressed(ofMouseEventArgs& event) {
		if (!bIsEnabled) return;
		if (glutGetModifiers() == GLUT_ACTIVE_SHIFT){
			bShiftDown = true;
		} else {
			bShiftDown = false;
		}
		if(mouseClicked == true)
			mouseButton = 3; // left + right
		else
			mouseButton = event.button;
		mouseClicked = true;
	}

	void mouseDragged(ofMouseEventArgs& event) {
		if (!bIsEnabled) return;
		int x = event.x;
		int y = event.y;
		if(mouseHasMoved) {
			dmouseX = ofLerp(pmouseX - x, dmouseX, startMomentum);
			dmouseY = ofLerp(pmouseY - y, dmouseY, startMomentum);
		} else
			mouseHasMoved = true;
		pmouseX = x;
		pmouseY = y;
	}

	void mouseReleased(ofMouseEventArgs& event) {
		if (!bIsEnabled) return;
		mouseHasMoved = false;
		mouseClicked = false;
	}
	
	//BR ADDED 4/28
	void toggleEnabled(){
		bIsEnabled = !bIsEnabled;
	}
	
	void enableMouse(){
		bIsEnabled = true;
	}
	
	void disableMouse(){
		bIsEnabled = false;
	}
	
	void saveSettings(){
		settings.loadFile("settings/camerasettings.xml");
		bool bSettings = settings.pushTag("settings");
		if (!bSettings){
			settings.addTag("settings");
			settings.pushTag("settings");
		}
		
		bool bPosition = settings.pushTag("position");
		if (!bPosition){
			settings.addTag("position");
			settings.pushTag("position");
		}				
		settings.setValue("x", getPosition().x, 0);
		settings.setValue("y", getPosition().y, 0);
		settings.setValue("z", getPosition().z, 0);
		settings.popTag();
		
		bool bEye = settings.pushTag("eye");
		if (!bEye){
			settings.addTag("eye");
			settings.pushTag("eye");
		}				
		settings.setValue("x", getEye().x, 0);
		settings.setValue("y", getEye().y, 0);
		settings.setValue("z", getEye().z, 0);
		settings.popTag();
		
		bool bUp = settings.pushTag("upVec");
		if (!bUp){
			settings.addTag("upVec");
			settings.pushTag("upVec");
		}				
		settings.setValue("x", getUp().x, 0);
		settings.setValue("y", getUp().y, 0);
		settings.setValue("z", getUp().z, 0);
		settings.popTag();
		
		settings.popTag();
		
		settings.saveFile("settings/camerasettings.xml");
	}
	
	void loadSettings(){
		bool loaded = settings.loadFile("settings/camerasettings.xml");
		if (loaded){
			if(settings.pushTag("settings")){
				
				ofxVec3f uVec;
				ofxVec3f pos;
				ofxVec3f eyeVal;
				
				if(settings.pushTag("position", 0)){
					pos.x = settings.getValue("x", 0, 0);
					pos.y = settings.getValue("y", 0, 0);
					pos.z = settings.getValue("z", 0, 0);
					position(pos);
					settings.popTag();
				}
				if (settings.pushTag("eye", 0)){
					eyeVal.x = settings.getValue("x", 0, 0);
					eyeVal.y = settings.getValue("y", 0, 0);
					eyeVal.z = settings.getValue("z", 0, 0);
					eye(eyeVal);
					settings.popTag();				
				}
				if (settings.pushTag("upVec", 0)){
					uVec.x = settings.getValue("x", 0, 0);
					uVec.y = settings.getValue("y", 0, 0);
					uVec.z = settings.getValue("z", 0, 0);
					settings.popTag();			
					up(uVec);	
				}
				
				
			}settings.popTag();
		}
	}
};

/*
 *  ofxLabUtils.h
 *  SJ_Transformer
 *
 *  Created by brenfer on 9/2/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */

#pragma once

static float ofDist3D(float x1, float y1, float z1, float x2, float y2, float z2 ) {
	float deltax=x2-x1;
	float deltay=y2-y1;
	float deltaz=z2-z1;
	return sqrt(double((deltax*deltax) + (deltay * deltay) + (deltaz * deltaz)));
}

//----------------------------------------------------------
// there has to be a way to include these OF functions... le sigh
static void labStartSmoothing(){
#ifndef TARGET_OPENGLES
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
#endif
	
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);
	
	//why do we need this?
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//----------------------------------------------------------
static void labEndSmoothing(){
#ifndef TARGET_OPENGLES
	glPopAttrib();
#endif
}

static float triangle3DPoints[9];

//----------------------------------------------------------
// ofTriangle... but in 3D!!!!!!!!!!!!!!!!!!!!!!!!!!
static void ofTriangle(float x1,float y1, float z1, float x2,float y2, float z2, float x3, float y3, float z3){
	
	// use smoothness, if requested:
	if (ofGetStyle().smoothing && !ofGetStyle().bFill) labStartSmoothing();
	
	// draw:
	triangle3DPoints[0] = x1;
	triangle3DPoints[1] = y1;
	triangle3DPoints[2] = z1;
	triangle3DPoints[3] = x2;
	triangle3DPoints[4] = y2;
	triangle3DPoints[5] = z2;
	triangle3DPoints[6] = x3;
	triangle3DPoints[7] = y3;
	triangle3DPoints[8] = z3;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &triangle3DPoints[0]);
	glDrawArrays((ofGetStyle().bFill) ? GL_TRIANGLES : GL_LINE_LOOP, 0, 3);
	
	// back to normal, if smoothness is on
	if (ofGetStyle().smoothing && !ofGetStyle().bFill) labEndSmoothing();
}

static float quadPoints[12];

static void ofQuad2D( float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4 )
{
	// use smoothness, if requested:
	if (ofGetStyle().smoothing && !ofGetStyle().bFill) labStartSmoothing();
	
	// draw:
	quadPoints[0] = x1;
	quadPoints[1] = y1;
	quadPoints[2] = 0;
	quadPoints[3] = x2;
	quadPoints[4] = y2;
	quadPoints[5] = 0;
	quadPoints[6] = x3;
	quadPoints[7] = y3;
	quadPoints[8] = 0;
	quadPoints[9] = x4;
	quadPoints[10] = y4;
	quadPoints[11] = 0;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &quadPoints[0]);
	glDrawArrays((ofGetStyle().bFill) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
	
	// back to normal, if smoothness is on
	if (ofGetStyle().smoothing && !ofGetStyle().bFill) labEndSmoothing();
};

static void ofQuad3D( float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float x4, float y4, float z4 )
{
	// use smoothness, if requested:
	if (ofGetStyle().smoothing && !ofGetStyle().bFill) labStartSmoothing();
	
	// draw:
	quadPoints[0] = x1;
	quadPoints[1] = y1;
	quadPoints[2] = z1;
	quadPoints[3] = x2;
	quadPoints[4] = y2;
	quadPoints[5] = z2;
	quadPoints[6] = x3;
	quadPoints[7] = y3;
	quadPoints[8] = z3;
	quadPoints[9] = x4;
	quadPoints[10] = y4;
	quadPoints[11] = z4;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &quadPoints[0]);
	glDrawArrays((ofGetStyle().bFill) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
	
	// back to normal, if smoothness is on
	if (ofGetStyle().smoothing && !ofGetStyle().bFill) labEndSmoothing();
};

static void ofQuad3D( ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4 )
{
	ofQuad3D(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z, p3.x, p3.y, p3.z, p4.x, p4.y, p4.z);
};

class ofQuad
{	
public:
	ofPoint points[4];
	
	void draw(){
		ofQuad3D(points[0], points[1], points[2],points[3]);
	}
};

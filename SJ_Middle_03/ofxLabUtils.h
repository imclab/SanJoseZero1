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
	return sqrt(double((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2)));
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
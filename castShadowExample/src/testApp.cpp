#include "testApp.h"

int fCounter;

GLuint depthTexture;

ofImage testImage;

bool toggle;

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground( 0, 0, 0 );
	ofSetVerticalSync(true);
	ofSetFrameRate(120);
	ofDisableArbTex();
	
	//shaders
	depthShader.setup("shaders/depthShader");
	simpleTex.setup("shaders/simpTex");
	
	//fbo
	//fbo.allocate(1024, 1024, GL_RGBA, 4);
	fbo.allocate(2048, 2048, true);
	fbo.clear(1.0, 1.0, 1.0, 1.f);
	testImage.loadImage("hop_3.png");
	
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	
	toggle = true; 

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofxVec3f lightPos(sin(mouseX * 0.01) * 1500,
					  mouseY*2.0,
					  cos(mouseX * 0.01) * 1500);
	ofxVec3f camPos(sin(ofGetElapsedTimef()* 0.1) * 1500.0,
					ofGetHeight()*0.5,
					cos(ofGetElapsedTimef()* 0.1) * 1500.0);
	
	camera.setClippingPlane(100.0, 5000.0);
	camera.setAspectRatio(1.0);
	camera.aimCamera(lightPos,
					 ofxVec3f(0.0,0.0,0.0));	
	
	fbo.begin();
	
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	
	camera.setAspectRatio(1.0);
	camera.aimCamera(lightPos,
					 ofxVec3f(0.0,0.0,0.0));
	
	depthShader.setUniform("lightPos", lightPos.x, lightPos.y, lightPos.z);
	depthShader.setUniform("shadeTex", 0);
	glActiveTexture(GL_TEXTURE0);
	testImage.getTextureReference().bind();
	depthShader.setUniform("colorTex", 1);
	glActiveTexture(GL_TEXTURE1);
	testImage.getTextureReference().bind();
	
	glEnable(GL_DEPTH_TEST);
	drawRandomGeometry(false);
	grabMatrix(GL_TEXTURE7);
	
	depthShader.end();
	
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					 0, 0, fbo.getWidth(), fbo.getHeight(),0);
	
	
	fbo.end();
	
	camera.setAspectRatio(float(ofGetWidth())/float(ofGetHeight()));
	camera.aimCamera(camPos,
					 ofxVec3f(0.0,0.0,0.0));
	
	//get modelViewMatrix and invert it
	float modelView[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	ofxMatrix4x4 invModelView(modelView);
	invModelView.makeInvertOf(invModelView);
	
	simpleTex.begin();
	//pass the inverse of modelViewMatrix to the shader
	glUniformMatrix4fv(glGetUniformLocationARB(simpleTex.shader, "invMat"),
					   1, GL_FALSE, invModelView.getPtr());
	
	simpleTex.setUniform("lightPos", lightPos.x, lightPos.y, lightPos.z);
		
	simpleTex.setUniform("ShadowMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	
	drawRandomGeometry(false);
	
	simpleTex.end();
	
	setOrthographicProjection(0, ofGetWidth(),
							  0, ofGetHeight(),
							  -1000, 1000);
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	drawTex(20, 20, 200, 200);
	//fbo.draw(20, 20, 250, 250);
	

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' ')toggle = !toggle;

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

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::setOrthographicProjection(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax) {
	
	glMatrixMode(GL_PROJECTION);// switch to projection mode	
	glPushMatrix();// save previous matrix which contains the settings for the perspective projection	
	glLoadIdentity();// reset matrix	
	glOrtho( xMin, xMax, yMin, yMax, zMin, zMax);// set a 2D orthographic projection//gluOrtho2D(0, w, 0, h);	
	glScalef(1, -1, 1);	// invert the y axis, down is positive
	glTranslatef(0, -yMax, 0);// mover the origin from the bottom left corner to the upper left corner
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
}

//--------------------------------------------------------------
void testApp::grabMatrix(GLenum gl_textureTarget){
	static double modelView[16];
	static double projection[16];
	
	// Moving from unit cube [-1,1] to [0,1]
	const GLdouble bias[16] = { 
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};
	
	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView); 
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(gl_textureTarget);
	glLoadIdentity();
	glLoadMatrixd(bias);
	
	// concatating all matrices into one. 
	glMultMatrixd (projection);
	glMultMatrixd (modelView);
	
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE5);
	glLoadIdentity(); 
	glLoadMatrixd(bias);
	glMultMatrixd (projection);
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE6);
	glLoadIdentity(); 
	glMultMatrixd (modelView);
	
	
	// Go back to normal matrix mode 
	glMatrixMode(GL_MODELVIEW);
}


//--------------------------------------------------------------
void testApp::drawTex(int x, int y, int w, int h){
	glNormal3f(0.0, 0.0, 1.0);
	ofSetColor(255,255,255);
	glBegin(GL_QUADS);	
	
	//glTexCoord2f(0.0, 1.0);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(x, y);	
	
	//glTexCoord2f(1.0, 1.0);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(x+w, y);	
	
	//glTexCoord2f(1.0, 0.0);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(x+w, y+h);	
	
	//glTexCoord2f(0.0, 0.0);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(x, y+h);	
	
	glEnd();		
	
}

//--------------------------------------------------------------
void testApp::gaussBlurKernel(int diameter, float *_gVals, float sigma, float _stepX, float _stepY){
	
	int radius = floor(float(diameter)/2.0);	
	double gaussSum = 0.0;
	int count = 0;
	
	for(int i=-radius; i<=radius;i++){
		for(int j=-radius; j<=radius; j++){
			double SD = sigma;
			double distX = j;
			double distY = i;
			
			float x = 1.0/(sqrt(2.0 * PI)*SD);
			float y = 1.0/(sqrt(2.0 * PI)*SD);
			x *= exp(-pow(distX, 2.0)/(2.0*pow(SD,2.0)));			
			y *= exp(-pow(distY, 2.0)/(2.0*pow(SD,2.0)));
			
			_gVals[count*3  ] = (float)j;
			_gVals[count*3+1] = (float)i;
			_gVals[count*3+2] = x*y;
			
			gaussSum += _gVals[count*3+2];			
			//cout<< _gVals[count*3+2] <<", ";
			count++;
		}
		//cout<<endl;
	}
	//cout<<"sum: "<<gaussSum<<endl;
	
	for(int i=0; i<=diameter*diameter;i++){
		_gVals[i*3+2] *= 1.0/gaussSum;		
		_gVals[i*3  ] *= _stepX;
		_gVals[i*3+1] *= _stepY;		
	}
}

void testApp::setTextureMatrix(void){
	static double modelView[16];
	static double projection[16];
	
	// Moving from unit cube [-1,1] to [0,1]  
	const GLdouble bias[16] = {	
		0.5, 0.0, 0.0, 0.0, 
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};
	
	// Grab modelview and transformation matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	
	glMatrixMode(GL_TEXTURE);
	glActiveTextureARB(GL_TEXTURE7);
	
	glLoadIdentity();	
	glLoadMatrixd(bias);
	
	// concatating all matrices into one.
	glMultMatrixd (projection);
	glMultMatrixd (modelView);
	
	// Go back to normal matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void testApp::drawRandomGeometry(bool grabM){
	for(int i=0; i<5; i++){
		for(int j=0; j<5;j++){
			for(int k=0; k<5; k++){
				
				if ((i+j+k)%2 == 0) {
					ofxSphere(i * 100 - 250,
							  j*100 - 250,
							  k*100 - 250,
							  80 + sin(ofGetElapsedTimef()+i+j+k)*35);
					/*ofxSphere(i * 100 - 250,
							 j*100 - 250,
							 k*100 - 250,
							 80 + sin(ofGetElapsedTimef() + i+j+k)*35,
							 grabM);*/
				}
				else{					
					ofxSphere(i * 100 - 250,
							  j*100 - 250,
							  k*100 - 250, 30 + 2.0*(i+j+k));
				}
			}
		}
	}
}

void testApp::drawABox(float x, float y, float z, float size, bool grabM){
	if(toggle)glPushMatrix();
	if(toggle)glTranslatef(x, y, z);
	
	size *= 0.5;
	ofxVec3f verts[8];
	verts[0].set(-size, size, -size);
	verts[1].set(-size, size, size);
	verts[2].set(size, size, size);
	verts[3].set(size, size, -size);
	
	verts[4].set(-size, -size, -size);
	verts[5].set(-size, -size, size);
	verts[6].set(size, -size, size);
	verts[7].set(size, -size, -size);
	
	if(!toggle){
		ofxVec3f pos(x, y, z);
		for(int i=0; i<8;i++)
			verts[i] += pos;
	}
	
	//top
	glBegin(GL_QUADS);
	glVertex3f(verts[0].x, verts[0].y, verts[0].z);
	glVertex3f(verts[1].x, verts[1].y, verts[1].z);
	glVertex3f(verts[2].x, verts[2].y, verts[2].z);
	glVertex3f(verts[3].x, verts[3].y, verts[3].z);
	
	//bottom
	glBegin(GL_QUADS);
	glVertex3f(verts[4].x, verts[4].y, verts[4].z);
	glVertex3f(verts[5].x, verts[5].y, verts[5].z);
	glVertex3f(verts[6].x, verts[6].y, verts[6].z);
	glVertex3f(verts[7].x, verts[7].y, verts[7].z);

	//side1
	glVertex3f(verts[0].x, verts[0].y, verts[0].z);
	glVertex3f(verts[1].x, verts[1].y, verts[1].z);
	glVertex3f(verts[5].x, verts[5].y, verts[5].z);
	glVertex3f(verts[4].x, verts[4].y, verts[4].z);
	
	//side2
	glVertex3f(verts[1].x, verts[1].y, verts[1].z);
	glVertex3f(verts[2].x, verts[2].y, verts[2].z);
	glVertex3f(verts[6].x, verts[6].y, verts[6].z);
	glVertex3f(verts[5].x, verts[5].y, verts[5].z);
	
	//side3
	glVertex3f(verts[2].x, verts[2].y, verts[2].z);
	glVertex3f(verts[3].x, verts[3].y, verts[3].z);
	glVertex3f(verts[7].x, verts[7].y, verts[7].z);
	glVertex3f(verts[6].x, verts[6].y, verts[6].z);
	
	//side4
	glVertex3f(verts[0].x, verts[0].y, verts[0].z);
	glVertex3f(verts[3].x, verts[3].y, verts[3].z);
	glVertex3f(verts[7].x, verts[7].y, verts[7].z);
	glVertex3f(verts[4].x, verts[4].y, verts[4].z);
	
	glEnd();
	if(grabM)grabMatrix(GL_TEXTURE7);
	if(toggle)glPopMatrix();
}
	
	
	
	

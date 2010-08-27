/*
 *  ShapeBatchRenderer.cpp
 *  plsSendRenderer
 *
 *  Created by Zach Gage on 5/21/10.
 *  Copyright 2010 stfj. All rights reserved.
 *  
 */
#include "ShapeBatchRenderer.h"

ShapeBatchRenderer::ShapeBatchRenderer(SHAPEMODE_t mode, int _numObjectsPerLayer, int _numLayers)
{
	numObjectsPerLayer	= _numObjectsPerLayer;
	numLayers			= _numLayers;
	
	verts				= new float[numLayers*numObjectsPerLayer*(mode*3)];
	colors				= new unsigned char[numLayers*numObjectsPerLayer*(mode*4)]; //4 colors for each vertex.
	numObjects			= new int[numLayers*numObjectsPerLayer];
	layerLineWidth		= new int[numLayers];
	
	for(int i=0;i<numLayers;i++)
		layerLineWidth[i] = 1;
	
	shapeMode			= mode;
	
	setCircleResolution(22);
	red=green=blue=alpha=255;
	safeMode=true;
	clear();
}

ShapeBatchRenderer::~ShapeBatchRenderer()
{
	delete verts;
	delete colors;
	delete numObjects;
	delete layerLineWidth;
}

//shapes --

bool ShapeBatchRenderer::addTriangle(ofPoint a, ofPoint b, ofPoint c, int layer)
{return addTriangle(a,red,green,blue,alpha, b,red,green,blue,alpha, c,red,green,blue,alpha, layer);}

bool ShapeBatchRenderer::addTriangle(ofPoint a, int aR, int aG, int aB, int aA, ofPoint b, int bR, int bG, int bB, int bA, ofPoint c, int cR, int cG, int cB, int cA, int layer)
{
	if(shapeMode!=SBR_TRIANGLE)
	{
		cerr<<"RENDER ERROR: Could not add triangle to ShapeBatchRender set up for lines!"<<endl;
		return false;
	}
	
	if(numObjects[layer] >= numObjectsPerLayer)
	{
		cerr << "RENDER ERROR: Layer " << layer << " over allocated! Max " << numObjectsPerLayer << " objects per layer!"  << endl;
		return false;
	}
	
	if(layer > numLayers)
	{
		cerr << "RENDER ERROR: Bogus layer '" << layer << "'! Only " << numLayers << " layers compiled!"  << endl;
		return false;
	}
	
	int layerOffset = layer * numObjectsPerLayer;
	int vertexOffset = (layerOffset + numObjects[layer]) * (shapeMode*3);
	int colorOffset = (layerOffset + numObjects[layer]) * (shapeMode*4);
		
	//verticies ------------------------------------
	verts[vertexOffset     ]	= a.x;
	verts[vertexOffset + 1 ]	= a.y;
	verts[vertexOffset + 2 ]	= a.z;
	
	verts[vertexOffset + 3 ]	= b.x;
	verts[vertexOffset + 4 ]	= b.y;
	verts[vertexOffset + 5 ]	= b.z;
	
	verts[vertexOffset + 6 ]	= c.x;
	verts[vertexOffset + 7 ]	= c.y;
	verts[vertexOffset + 8 ]	= c.z;
	
	//colors ---------------------------------------
	colors[colorOffset	   ]	= aR;
	colors[colorOffset + 1 ]	= aG;
	colors[colorOffset + 2 ]	= aB;
	colors[colorOffset + 3 ]	= aA;
	
	colors[colorOffset + 4 ]	= bR;
	colors[colorOffset + 5 ]	= bG;
	colors[colorOffset + 6 ]	= bB;
	colors[colorOffset + 7 ]	= bA;
	
	colors[colorOffset + 8 ]	= cR;
	colors[colorOffset + 9 ]	= cG;
	colors[colorOffset + 10]	= cB;
	colors[colorOffset + 11]	= cA;
	
	numObjects[layer]++;
	
	return true;
}

bool ShapeBatchRenderer::addLine(ofPoint a, ofPoint b, int layer)
{return addLine(a,red,green,blue,alpha, b,red,green,blue,alpha, layer);}

bool ShapeBatchRenderer::addLine(ofPoint a, int aR, int aG, int aB, int aA, ofPoint b, int bR, int bG, int bB, int bA, int layer)
{
	if(shapeMode!=SBR_LINE)
	{
		cerr<<"RENDER ERROR: Could not add line to ShapeBatchRender set up for triangles!"<<endl;
		return false;
	}
	
	if(numObjects[layer] >= numObjectsPerLayer)
	{
		cerr << "RENDER ERROR: Layer " << layer << " over allocated! Max " << numObjectsPerLayer << " objects per layer!"  << endl;
		return false;
	}
	
	if(layer > numLayers)
	{
		cerr << "RENDER ERROR: Bogus layer '" << layer << "'! Only " << numLayers << " layers compiled!"  << endl;
		return false;
	}
	
	int layerOffset = layer * numObjectsPerLayer;
	int vertexOffset = (layerOffset + numObjects[layer]) * (shapeMode*3);
	int colorOffset = (layerOffset + numObjects[layer]) * (shapeMode*4);
	
	//verticies ------------------------------------
	verts[vertexOffset     ]	= a.x;
	verts[vertexOffset + 1 ]	= a.y;
	verts[vertexOffset + 2 ]	= a.z;
	
	verts[vertexOffset + 3 ]	= b.x;
	verts[vertexOffset + 4 ]	= b.y;
	verts[vertexOffset + 5 ]	= b.z;
	
	//colors ---------------------------------------
	colors[colorOffset	   ]	= aR;
	colors[colorOffset + 1 ]	= aG;
	colors[colorOffset + 2 ]	= aB;
	colors[colorOffset + 3 ]	= aA;
	
	colors[colorOffset + 4 ]	= bR;
	colors[colorOffset + 5 ]	= bG;
	colors[colorOffset + 6 ]	= bB;
	colors[colorOffset + 7 ]	= bA;
	
	numObjects[layer]++;
	
	return true;
}


//handled --
bool ShapeBatchRenderer::addCircle(float x, float y, float z, float radius, int layer)
{return addElipse(x,y,z,radius,radius,layer);}

bool ShapeBatchRenderer::addElipse(float x, float y, float z, float w, float h, int layer)
{
	if(layer > numLayers)
	{
		cerr << "RENDER ERROR: Bogus layer '" << layer << "'! Only " << numLayers << " layers compiled!"  << endl;
		return false;
	}
	
	int layerOffset = layer * numObjectsPerLayer;
	int vertexOffset;
	int colorOffset;
	
	if(shapeMode == SBR_LINE)
	{
		int k = 0;
		for(int i=0;i<numCirclePts;i++)
		{
			vertexOffset = (layerOffset + numObjects[layer]) *(shapeMode*3);
			colorOffset = (layerOffset + numObjects[layer]) * (shapeMode*4);
			
			if(numObjects[layer] >= numObjectsPerLayer)
			{
				cerr << "RENDER ERROR: Layer " << layer << " over allocated! Max " << numObjectsPerLayer << " objects per layer! Ran out during circle addition"  << endl;
				return false;
			}
			
			if(i>0)
			{
				//verticies ------------------------------------
				verts[vertexOffset     ]	= x + circlePts[k] * w  * 0.5;
				verts[vertexOffset + 1 ]	= y + circlePts[k+1] * h  * 0.5;
				verts[vertexOffset + 2 ]	= z;
				
				verts[vertexOffset + 3 ]	= x + circlePts[k-2] * w  * 0.5;
				verts[vertexOffset + 4 ]	= y + circlePts[k-1] * h  * 0.5;
				verts[vertexOffset + 5 ]	= z;
			}
			else if(i==0)
			{
				//verticies ------------------------------------
				verts[vertexOffset     ]	= x + circlePts[k] * w  * 0.5;
				verts[vertexOffset + 1 ]	= y + circlePts[k+1] * h  * 0.5;
				verts[vertexOffset + 2 ]	= z;
				
				verts[vertexOffset + 3 ]	= x + circlePts[numCirclePts*2-2] * w  * 0.5;
				verts[vertexOffset + 4 ]	= y + circlePts[numCirclePts*2-1] * h  * 0.5;
				verts[vertexOffset + 5 ]	= z;
			}
			
			//colors ---------------------------------------
			colors[colorOffset	   ]	= red;
			colors[colorOffset + 1 ]	= green;
			colors[colorOffset + 2 ]	= blue;
			colors[colorOffset + 3 ]	= alpha;
			
			colors[colorOffset + 4 ]	= red;
			colors[colorOffset + 5 ]	= green;
			colors[colorOffset + 6 ]	= blue;
			colors[colorOffset + 7 ]	= alpha;
			
			numObjects[layer]++;
			k+=2;
		}
	}
	else if(shapeMode == SBR_TRIANGLE)
	{
		int k = 0;
		
		for(int i=0;i<numCirclePts;i++)
		{
			vertexOffset = (layerOffset + numObjects[layer])*(shapeMode*3);
			colorOffset = (layerOffset + numObjects[layer])*(shapeMode*4);
			
			if(numObjects[layer] >= numObjectsPerLayer)
			{
				cerr << "RENDER ERROR: Layer " << layer << " over allocated! Max " << numObjectsPerLayer << " objects per layer! Ran out during circle addition"  << endl;
				return false;
			}
			
			if(i>0)
			{
				//verticies ------------------------------------
				verts[vertexOffset     ]	= x + circlePts[k] * w  * 0.5;
				verts[vertexOffset + 1 ]	= y + circlePts[k+1] * h  * 0.5;
				verts[vertexOffset + 2 ]	= z;
				
				verts[vertexOffset + 3 ]	= x + circlePts[k-2] * w  * 0.5;
				verts[vertexOffset + 4 ]	= y + circlePts[k-1] * h  * 0.5;
				verts[vertexOffset + 5 ]	= z;
				
				verts[vertexOffset + 6 ]	= x;
				verts[vertexOffset + 7 ]	= y;
				verts[vertexOffset + 8 ]	= z;
			}
			else if(i==0)
			{
				//verticies ------------------------------------
				verts[vertexOffset     ]	= x + circlePts[k] * w  * 0.5;
				verts[vertexOffset + 1 ]	= y + circlePts[k+1] * h  * 0.5;
				verts[vertexOffset + 2 ]	= z;
				
				verts[vertexOffset + 3 ]	= x + circlePts[numCirclePts*2-2] * w  * 0.5;
				verts[vertexOffset + 4 ]	= y + circlePts[numCirclePts*2-1] * h  * 0.5;
				verts[vertexOffset + 5 ]	= z;
				
				verts[vertexOffset + 6 ]	= x;
				verts[vertexOffset + 7 ]	= y;
				verts[vertexOffset + 8 ]	= z;
			}
			
			//colors ---------------------------------------
			colors[colorOffset	   ]	= red;
			colors[colorOffset + 1 ]	= green;
			colors[colorOffset + 2 ]	= blue;
			colors[colorOffset + 3 ]	= alpha;
			
			colors[colorOffset + 4 ]	= red;
			colors[colorOffset + 5 ]	= green;
			colors[colorOffset + 6 ]	= blue;
			colors[colorOffset + 7 ]	= alpha;
			
			colors[colorOffset + 8 ]	= red;
			colors[colorOffset + 9 ]	= green;
			colors[colorOffset + 10]	= blue;
			colors[colorOffset + 11]	= alpha;
			
			numObjects[layer]++;
			k+=2;
		}
	}
	
	return true;
}

bool ShapeBatchRenderer::addRect(float x, float y, float z, float w, float h, int layer)
{
	if(layer > numLayers)
	{
		cerr << "RENDER ERROR: Bogus layer '" << layer << "'! Only " << numLayers << " layers compiled!"  << endl;
		return false;
	}
	
	int layerOffset = layer * numObjectsPerLayer;
	int vertexOffset = (layerOffset + numObjects[layer]) * (shapeMode*3);
	int colorOffset = (layerOffset + numObjects[layer]) * (shapeMode*4);
	
	if(shapeMode == SBR_LINE)
	{
		if(numObjects[layer]+4 >= numObjectsPerLayer)
		{
			cerr << "RENDER ERROR: Layer " << layer << " over allocated! Max " << numObjectsPerLayer << " objects per layer! Ran out during rect addition"  << endl;
			return false;
		}
		
		//verticies ------------------------------------
		verts[vertexOffset     ]	= x;
		verts[vertexOffset + 1 ]	= y;
		verts[vertexOffset + 2 ]	= z;
		
		verts[vertexOffset + 3 ]	= x + w;
		verts[vertexOffset + 4 ]	= y;
		verts[vertexOffset + 5 ]	= z;
		
		
		verts[vertexOffset + 6 ]	= x;
		verts[vertexOffset + 7 ]	= y;
		verts[vertexOffset + 8 ]	= z;
		
		verts[vertexOffset + 9 ]	= x;
		verts[vertexOffset + 10]	= y + h;
		verts[vertexOffset + 11]	= z;
		
		
		verts[vertexOffset + 12]	= x + w;
		verts[vertexOffset + 13]	= y;
		verts[vertexOffset + 14]	= z;
		
		verts[vertexOffset + 15]	= x + w;
		verts[vertexOffset + 16]	= y + h;
		verts[vertexOffset + 17]	= z;
		
		
		verts[vertexOffset + 18]	= x;
		verts[vertexOffset + 19]	= y + h;
		verts[vertexOffset + 20]	= z;
		
		verts[vertexOffset + 21]	= x + w;
		verts[vertexOffset + 22]	= y + h;
		verts[vertexOffset + 23]	= z;
		
		//colors ---------------------------------------
		colors[colorOffset	   ]	= red;
		colors[colorOffset + 1 ]	= green;
		colors[colorOffset + 2 ]	= blue;
		colors[colorOffset + 3 ]	= alpha;
		
		colors[colorOffset + 4 ]	= red;
		colors[colorOffset + 5 ]	= green;
		colors[colorOffset + 6 ]	= blue;
		colors[colorOffset + 7 ]	= alpha;
		
		
		colors[colorOffset + 8 ]	= red;
		colors[colorOffset + 9 ]	= green;
		colors[colorOffset + 10]	= blue;
		colors[colorOffset + 11]	= alpha;
		
		colors[colorOffset + 12]	= red;
		colors[colorOffset + 13]	= green;
		colors[colorOffset + 14]	= blue;
		colors[colorOffset + 15]	= alpha;
		
		
		colors[colorOffset + 16]	= red;
		colors[colorOffset + 17]	= green;
		colors[colorOffset + 18]	= blue;
		colors[colorOffset + 19]	= alpha;
		
		colors[colorOffset + 20]	= red;
		colors[colorOffset + 21]	= green;
		colors[colorOffset + 22]	= blue;
		colors[colorOffset + 23]	= alpha;
		
		
		colors[colorOffset + 24]	= red;
		colors[colorOffset + 25]	= green;
		colors[colorOffset + 26]	= blue;
		colors[colorOffset + 27]	= alpha;
		
		colors[colorOffset + 28]	= red;
		colors[colorOffset + 29]	= green;
		colors[colorOffset + 30]	= blue;
		colors[colorOffset + 31]	= alpha;
		
		numObjects[layer]+=4;
	}
	else if(shapeMode == SBR_TRIANGLE)
	{
		if(numObjects[layer]+2 >= numObjectsPerLayer)
		{
			cerr << "RENDER ERROR: Layer " << layer << " over allocated! Max " << numObjectsPerLayer << " objects per layer! Ran out during rect addition"  << endl;
			return false;
		}
		
		//verticies ------------------------------------
		verts[vertexOffset     ]	= x;
		verts[vertexOffset + 1 ]	= y;
		verts[vertexOffset + 2 ]	= z;
		
		verts[vertexOffset + 3 ]	= x + w;
		verts[vertexOffset + 4 ]	= y;
		verts[vertexOffset + 5 ]	= z;
		
		verts[vertexOffset + 6 ]	= x;
		verts[vertexOffset + 7 ]	= y + h;
		verts[vertexOffset + 8 ]	= z;
		
		
		verts[vertexOffset + 9 ]	= x + w;
		verts[vertexOffset + 10]	= y;
		verts[vertexOffset + 11]	= z;
	
		verts[vertexOffset + 12]	= x;
		verts[vertexOffset + 13]	= y + h;
		verts[vertexOffset + 14]	= z;
		
		verts[vertexOffset + 15]	= x + w;
		verts[vertexOffset + 16]	= y + h;
		verts[vertexOffset + 17]	= z;
			
		//colors ---------------------------------------
		colors[colorOffset	   ]	= red;
		colors[colorOffset + 1 ]	= green;
		colors[colorOffset + 2 ]	= blue;
		colors[colorOffset + 3 ]	= alpha;
		
		colors[colorOffset + 4 ]	= red;
		colors[colorOffset + 5 ]	= green;
		colors[colorOffset + 6 ]	= blue;
		colors[colorOffset + 7 ]	= alpha;
		
		colors[colorOffset + 8 ]	= red;
		colors[colorOffset + 9 ]	= green;
		colors[colorOffset + 10]	= blue;
		colors[colorOffset + 11]	= alpha;
		
		
		colors[colorOffset + 12]	= red;
		colors[colorOffset + 13]	= green;
		colors[colorOffset + 14]	= blue;
		colors[colorOffset + 15]	= alpha;
		
		colors[colorOffset + 16]	= red;
		colors[colorOffset + 17]	= green;
		colors[colorOffset + 18]	= blue;
		colors[colorOffset + 19]	= alpha;
		
		colors[colorOffset + 20]	= red;
		colors[colorOffset + 21]	= green;
		colors[colorOffset + 22]	= blue;
		colors[colorOffset + 23]	= alpha;
		
		numObjects[layer]+=2;
	}
	
	return true;
}

bool ShapeBatchRenderer::addCenteredRect(float x, float y, float z, float w, float h, int layer)
{return addRect(x-w/2,y-h/2,z,w,h,layer);}

//functions --


void ShapeBatchRenderer::clear()
{
	for(int i = 0; i < numLayers; i++) numObjects[i] = 0;
}

void ShapeBatchRenderer::draw()
{
	if(safeMode)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	
	glVertexPointer(3, GL_FLOAT, 0, &verts[0]);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, &colors[0]);
	
	for(int l = 0; l < numLayers; l++)
	{
		if(numObjects[l] > 0)
		{
			glLineWidth(layerLineWidth[l]);
			
			if(shapeMode == SBR_LINE)
				glDrawArrays(GL_LINES, l*numObjectsPerLayer*shapeMode, numObjects[l]*shapeMode);
			else if(shapeMode == SBR_TRIANGLE)
				glDrawArrays(GL_TRIANGLES, l*numObjectsPerLayer*shapeMode, numObjects[l]*shapeMode);
		}
	}
	
	if(safeMode)
	{
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void ShapeBatchRenderer::setColor(int r, int g, int b, int a)
{
	red		= r;
	green	= g;
	blue	= b;
	alpha	= a;
}

void ShapeBatchRenderer::setCircleResolution(int res){
	res = MIN( MAX(1, res), OF_MAX_CIRCLE_PTS);
	
	if (res > 1 && res != numCirclePts){
		numCirclePts = res;
		
		float angle = 0.0f;
		float angleAdder = M_TWO_PI / (float)res;
		int k = 0;
		for (int i = 0; i < numCirclePts; i++){
			circlePts[k] = cos(angle);
			circlePts[k+1] = sin(angle);
			angle += angleAdder;
			k+=2;
		}
		bSetupCircle = true;
	}
}

void ShapeBatchRenderer::setLayerLineWidth(int layer, int lineWidth)
{
	layerLineWidth[layer]=lineWidth;
}

void ShapeBatchRenderer::setSafeMode(bool safe)
{
	safeMode=safe;
}
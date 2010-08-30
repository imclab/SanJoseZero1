/*
 *  ofxLabProjectionOverlay.h
 *
 *  Created by brenfer on 7/14/10.
 *  Copyright 2010 Rockwell Group. All rights reserved.
 *
 */
class ofxLabProjectionOverlay
{
public:
	
	float rotation;
	ofPoint pos;
	float x, y, width, height;
	float minX, minY, maxX, maxY;
	int localIndex;
	
	ofxLabProjectionOverlay(){ 
		localIndex = 0;
		rotation = 0; 
		bHasImage = false;
		bShapeCreated = false;
		
		bSelected = false;
		wallColor.r = wallColor.g = wallColor.b = 0;
		activeColor.r = 150;
		activeColor.g = activeColor.b = 0;
		//width = height = baseX = baseY = x = y = 0;
		
		for (int i=0; i<4; i++){
			ofPoint *edge = new ofPoint(0,0);
			edges.push_back(edge);
		}
	};
	
	void setWallColor( int r, int g, int b ){
		wallColor.r = r;
		wallColor.g = g;
		wallColor.b = b;
	}		
	
	ofPoint getEdge(int which){
		return *edges[which];
	}	
	
	int getNumEdges(){
		return edges.size();
	}
	
	void bind(){
		texture.bind();
	}
	
	void unbind(){
		texture.unbind();
	}
	
	void setPosition( float x, float y){
		pos.x = x;
		pos.y = y;
		//bShapeCreated = false;
		//reset();
		
		minX = 9999;
		maxX = -9999;
		minY = 9999;
		maxY = -9999;
		
		//sort(edges.begin(), edges.end(), &compare);
		
		for (int i=0; i<edges.size(); i++){
			//addVertex(pos.x + edges[i]->x, pos.y + edges[i]->y);
			if (pos.x + edges[i]->x < minX) minX = pos.x + edges[i]->x;
			if (pos.x + edges[i]->x > maxX) maxX = pos.x + edges[i]->x;
			if (pos.y + edges[i]->y < minY) minY = pos.y + edges[i]->y;
			if (pos.y + edges[i]->y > maxY) maxY = pos.y + edges[i]->y;
		};
	};
	
	// call this on mouse press to reset relative points
	void resetPositioning( float x, float y){
		for (int i=0; i<edges.size(); i++){
			edges[i]->x += pos.x;
			edges[i]->y += pos.y;
		}
		
		pos.x = x;
		pos.y = y;
		
		for (int i=0; i<edges.size(); i++){
			edges[i]->x -= pos.x;
			edges[i]->y -= pos.y;
		}
	};
	
	static bool compare(ofPoint * first, ofPoint * second){
		return first->x > second->x && first->y < second->y;
	}
	
	void addPoint( float x, float y){
		if (localIndex < edges.size()){
			setEdge(x, y, localIndex);
		} else {
			ofPoint *p = new ofPoint(x,y);
			edges.push_back(p);
			setEdge(x, y, localIndex);
		}
		localIndex++;	
		bShapeCreated = false;
	};
	
	void setEdge( float x, float y, int which){
		edges[which]->x = x;
		edges[which]->y = y;	
		bShapeCreated = false;
	}
	
	void loadImage( string img, int _imageWidth, int _imageHeight ){
		imageWidth = _imageWidth;
		imageHeight = _imageHeight;
		bHasImage = image.loadImage(img);
		if (bHasImage) texture.allocate(imageWidth, imageHeight, GL_RGBA);
		width = imageWidth;
		height = imageHeight;
	};
	
	void render(){
		if (bHasImage){
			texture.swapIn();
			texture.clear();
			
			ofPushMatrix();{
				ofTranslate(x, y);
				ofRotateZ(rotation);
				//ofTranslate(-width/2.0, -height/2.0);
				image.draw(0,0,width, height);
			}ofPopMatrix();
			texture.swapOut();
		} else {
			if (!bShapeCreated){
				//begin(GL_POLYGON);
				minX = 9999;
				maxX = -9999;
				minY = 9999;
				maxY = -9999;
				
				//sort(edges.begin(), edges.end(), &compare);
				
				for (int i=0; i<edges.size(); i++){
					//addVertex(pos.x + edges[i]->x, pos.y + edges[i]->y);
					if (pos.x + edges[i]->x < minX) minX = pos.x + edges[i]->x;
					if (pos.x + edges[i]->x > maxX) maxX = pos.x + edges[i]->x;
					if (pos.y + edges[i]->y < minY) minY = pos.y + edges[i]->y;
					if (pos.y + edges[i]->y > maxY) maxY = pos.y + edges[i]->y;
				};
				
				for (int i=0; i<edges.size(); i++){
					edges[i]->x += pos.x;
					edges[i]->y += pos.y;
				}
				
				pos.x = minX;
				pos.y = minY;
				
				for (int i=0; i<edges.size(); i++){
					edges[i]->x -= pos.x;
					edges[i]->y -= pos.y;
				}
				
				bShapeCreated = true;
				width = maxX - minX;
				height = maxY - minY;
			};
		}
	};
	
	void draw(){
		if (!bHasImage){
			if (!bSelected) ofSetColor(wallColor.r, wallColor.g, wallColor.b);
			else ofSetColor(activeColor.r, activeColor.g, activeColor.b);
			//if (bShapeCreated) end();
			//else {
				ofBeginShape();{
					for (int i=0; i<edges.size(); i++){
						ofVertex(pos.x + edges[i]->x, pos.y + edges[i]->y);
					};
				}ofEndShape(true);
				//bShapeCreated = false;
			//}
			ofSetColor(0xffffff);
		} else {
			//texture.draw(0,0);
		}
	}
	
	//from MSAInteractiveObject
	
	bool bSelected;
	bool hitTest(int tx, int ty) {		
		bSelected = ((tx > minX) && (tx < maxX) && (ty > minY) && (ty < maxY));
		return bSelected;
	}
	
private:
	
	ofImage image;
	ofxFBOTexture texture;
	int imageWidth, imageHeight;
	
	vector<ofPoint *> edges;
	float totalWidth, totalHeight;
	
	bool bHasImage;
	bool bShapeCreated;
	
	ofColor activeColor;
	ofColor wallColor;
	
	//mouse things
	
};
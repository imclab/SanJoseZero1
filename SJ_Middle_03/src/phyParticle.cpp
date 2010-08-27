#include "phyParticle.h"

//------------------------------------------------------------
phyParticle::phyParticle(){
	setInitialCondition(0,0,0,0);
	damping = 0.01f;
	rot=0;
	rotChange=ofRandomf();
	textures=NULL;
	type = ofRandomuf()*10;
	
	mass=1;
}

//------------------------------------------------------------
void phyParticle::resetForce(){
    // we reset the forces every frame
    frc.set(0,0);
}

//------------------------------------------------------------
void phyParticle::addForce(float x, float y){
    // add in a force in X and Y for this frame.
    frc.x = frc.x + x;
    frc.y = frc.y + y;
}

//------------------------------------------------------------
void phyParticle::addRepulsionForce(float x, float y, float radius, float scale){
    
	// ----------- (1) make a vector of where this position is: 
	
	ofxVec2f posOfForce;
	posOfForce.set(x,y);
	
	// ----------- (2) calculate the difference & length 
	
	ofxVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
        diff.normalize();
		frc.x = frc.x + diff.x * scale * pct;
        frc.y = frc.y + diff.y * scale * pct;
    }
}

//------------------------------------------------------------
void phyParticle::addAttractionForce(float x, float y, float radius, float scale){
    
	// ----------- (1) make a vector of where this position is: 
	
	ofxVec2f posOfForce;
	posOfForce.set(x,y);
	
	// ----------- (2) calculate the difference & length 
	
	ofxVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x - diff.x * scale * pct;
        frc.y = frc.y - diff.y * scale * pct;
    }
}

//------------------------------------------------------------
void phyParticle::addRepulsionForce(phyParticle &p, float radius, float scale){
	
	// ----------- (1) make a vector of where this phyParticle p is: 
	ofxVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofxVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x + diff.x * scale * pct;
        frc.y = frc.y + diff.y * scale * pct;
		p.frc.x = p.frc.x - diff.x * scale * pct;
        p.frc.y = p.frc.y - diff.y * scale * pct;
    }
}

//------------------------------------------------------------
void phyParticle::addAttractionForce(phyParticle & p, float radius, float scale){
	
	// ----------- (1) make a vector of where this phyParticle p is: 
	ofxVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofxVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x - diff.x * scale * pct;
        frc.y = frc.y - diff.y * scale * pct;
		p.frc.x = p.frc.x + diff.x * scale * pct;
        p.frc.y = p.frc.y + diff.y * scale * pct;
    }
	
}

//------------------------------------------------------------
void phyParticle::addClockwiseForce(phyParticle &p, float radius, float scale){
	
	rotChange = fabs(rotChange);
	
	// ----------- (1) make a vector of where this phyParticle p is: 
	ofxVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofxVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x - diff.y * scale * pct;
        frc.y = frc.y + diff.x * scale * pct;
		p.frc.x = p.frc.x + diff.y * scale * pct;
        p.frc.y = p.frc.y - diff.x * scale * pct;
    }
}

//------------------------------------------------------------
void phyParticle::addCounterClockwiseForce(phyParticle &p, float radius, float scale){
	
	rotChange = fabs(rotChange)*-1;
	
	// ----------- (1) make a vector of where this phyParticle p is: 
	ofxVec2f posOfForce;
	posOfForce.set(p.pos.x,p.pos.y);
	
	// ----------- (2) calculate the difference & length 
	
	ofxVec2f diff	= pos - posOfForce;
	float length	= diff.length();
	
	// ----------- (3) check close enough
	
	bool bAmCloseEnough = true;
    if (radius > 0){
        if (length > radius){
            bAmCloseEnough = false;
        }
    }
	
	// ----------- (4) if so, update force
    
	if (bAmCloseEnough == true){
		float pct = 1 - (length / radius);  // stronger on the inside
		diff.normalize();
		frc.x = frc.x + diff.y * scale * pct;
        frc.y = frc.y - diff.x * scale * pct;
		p.frc.x = p.frc.x - diff.y * scale * pct;
        p.frc.y = p.frc.y + diff.x * scale * pct;
		
    }
}


//------------------------------------------------------------
void phyParticle::addDampingForce(){
	
	// the usual way to write this is  vel *= 0.99
	// basically, subtract some part of the velocity 
	// damping is a force operating in the oposite direction of the 
	// velocity vector
	
    frc.x = frc.x - vel.x * damping;
    frc.y = frc.y - vel.y * damping;
}

//------------------------------------------------------------
void phyParticle::setInitialCondition(float px, float py, float vx, float vy){
    pos.set(px,py);
	vel.set(vx,vy);
}

//------------------------------------------------------------
void phyParticle::update(){	
	vel = vel + frc;
	pos = pos + vel;
	
	trail.push_back(ofPoint(pos.x,pos.y));
	
	if(trail.size()>20)
	{
		trail.erase(trail.begin());
	}
}

//------------------------------------------------------------
void phyParticle::draw(){
   // ofCircle(pos.x, pos.y, 1);
	rot+=rotChange;
	ofSetColor(255,255,255,80);
	
	
	ofNoFill();
	//ofBeginShape();
	//glBegin(GL_LINE_STRIP);
	for(int i=1;i<trail.size();i++)
	{
		glLineWidth(i);
		//ofLine(trail[i].x,trail[i].y,trail[i-1].x,trail[i-1].y);
		//ofVertex(trail[i].x,trail[i].y);
		//glVertex3f(trail[i].x, trail[i].y, -pow((100-i), 2.0));
	}
	ofSetColor(255,255,255);
	glLineWidth(1);
	
	//ofFill();
	ofSetRectMode(OF_RECTMODE_CENTER);
	glPushMatrix();
	glTranslated(pos.x,pos.y,0);
	glRotatef(rot,0,0,1);
	if(textures==NULL)
		ofRect(0,0,20,20);
	else
		textures[type].draw(0,0,20,20);
	
	
	glLineWidth(1);
	glPopMatrix();
	
	//ofSetColor(0,0,0,50);
	
	//glEnd();
	//ofEndShape(false);
}


//------------------------------------------------------------
void phyParticle::bounceOffWalls(){
	
	// sometimes it makes sense to damped, when we hit
	bool bDampedOnCollision = false;
	bool bDidICollide = false;
	
	// what are the walls
	float minx = 0;
	float miny = 0;
	float maxx = ofGetWidth();
	float maxy = ofGetHeight();
	
	if (pos.x > maxx){
		pos.x = maxx; // move to the edge, (important!)
		vel.x *= -1;
		bDidICollide = true;
	} else if (pos.x < minx){
		pos.x = minx; // move to the edge, (important!)
		vel.x *= -1;
		bDidICollide = true;
	}
	
	if (pos.y > maxy){
		pos.y = maxy; // move to the edge, (important!)
		vel.y *= -1;
		bDidICollide = true;
	} else if (pos.y < miny){
		pos.y = miny; // move to the edge, (important!)
		vel.y *= -1;
		bDidICollide = true;
	}
	
	if (bDidICollide == true && bDampedOnCollision == true){
		vel *= 0.3;
	}
	
}

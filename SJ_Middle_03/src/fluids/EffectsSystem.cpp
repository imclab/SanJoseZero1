#include "EffectsSystem.h"

#include "EffectsSystem.h"

using namespace MSA;

//--------------------------------------------------------------
void EffectsSystem::setup(Emitter * e) {	 
	
	emitter = e;
	
	// setup fluid stuff
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.03).setDeltaT(0.75).setVisc(0.00015).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	particleSystem.setFluidSolver( &fluidSolver );
	
	fluidCellsX			= 100;
	
	drawFluid			= true;
	drawParticles		= true;

#ifdef USE_GUI 
	gui.addSlider("fluidCellsX", fluidCellsX, 20, 400);
	gui.addButton("resizeFluid", resizeFluid);
	gui.addSlider("fs.viscocity", fluidSolver.viscocity, 0.0, 0.01, 0.5); 
	gui.addSlider("fs.colorDiffusion", fluidSolver.colorDiffusion, 0.0, 0.0003, 0.5); 
	gui.addSlider("fs.fadeSpeed", fluidSolver.fadeSpeed, 0.0, 0.1, 0.5); 
	gui.addSlider("fs.solverIterations", fluidSolver.solverIterations, 1, 50); 
	gui.addSlider("fs.deltaT", fluidSolver.deltaT, 0.1, 5);
	gui.addComboBox("fd.drawMode", (int&)fluidDrawer.drawMode, kFluidDrawCount, (string*)FluidDrawerGl::drawOptionTitles);
	gui.addToggle("fs.doRGB", fluidSolver.doRGB); 
	gui.addToggle("fs.doVorticityConfinement", fluidSolver.doVorticityConfinement); 
	gui.addToggle("drawFluid", drawFluid); 
	gui.addToggle("drawParticles", drawParticles); 
	gui.addToggle("fs.wrapX", fluidSolver.wrap_x); 
	gui.addToggle("fs.wrapY", fluidSolver.wrap_y);
	gui.setDefaultKeys(true);
	gui.currentPage().setXMLName("MSAFluidDemo.xml");
	gui.setAutoSave(false);
#endif
	
	windowResized(ofGetWidth(), ofGetHeight());		// force this at start (cos I don't think it is called)
	resizeFluid			= true;
	
	ofEnableAlphaBlending();
	ofSetBackgroundAuto(true);
	showSettings=false;
	loc.x = 0;
	
	particleSystem.setBuffer(emitter->getTransformEnd(), emitter->getTransformStart());
}


void EffectsSystem::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}


// add force and dye to fluid, and create particles
void EffectsSystem::addToFluid( Vec2f pos, Vec2f vel, ofColor color, bool addColor, bool addForce ) {
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if(speed > 0) {
		pos.x = constrain(pos.x, 0.0f, 1.0f);
		pos.y = constrain(pos.y, 0.0f, 1.0f);
		
        const float colorMult = 100;
        const float velocityMult = 30;
		
        int index = fluidSolver.getIndexForPos(pos);
		
		if(addColor) {
			Color drawColor( CM_RGB, color.r/255.0f, color.g/255.0f, color.b/255.0f );
			
			fluidSolver.addColorAtIndex(index, drawColor * 2.0f);
			
			if( drawParticles )
				particleSystem.addParticles( pos * Vec2f( getWindowSize() ), ofRandom(10,50) );
		}
		
		if(addForce)
			fluidSolver.addForceAtIndex(index, vel * velocityMult);
		
    }
}


void EffectsSystem::update(){
	
	particleSystem.setBuffer(emitter->getTransformEnd(), emitter->getTransformStart());
	/*if (windowResized( ofGetWidth(), emitter->getTransformStart() - emitter->getTransformEnd())){
		resizeFluid = true;
		loc.y = emitter->getTransformEnd();
	}*/
	
	if(resizeFluid) 	{
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}
	
	for(int i=0;i<emitter->particles.size();i++)
	{
		if (emitter->particles[i]->bTransforming) {
			addToFluid( Vec2f( (ofRandomf()*emitter->particles[i]->getWidth()/2+ emitter->particles[i]->getRotatedLoc().x)/ofGetWidth(), 
							  (ofRandomf()*emitter->particles[i]->getHeight()/2+(emitter->particles[i]->getRotatedLoc().y))/ofGetHeight()), 
					   Vec2f(emitter->particles[i]->vel.normalized().x/30.0f, emitter->particles[i]->vel.normalized().y/30.0f), emitter->particles[i]->color, true, true);
		}
	}
	
	fluidSolver.update();
}

void EffectsSystem::draw(){
	ofPushMatrix();{
		if( drawFluid ) {
			glColor3f(1, 1, 1);
			fluidDrawer.draw(loc.x, loc.y, window.x, window.y);
		} else {
			if(getElapsedFrames()%5==0) fadeToColor( 0, 0, 0, 0.1f );
		}
		ofTranslate(loc.x, loc.y);
		if( drawParticles )
			particleSystem.updateAndDraw( drawFluid );
		
	} ofPopMatrix();
	
#ifdef USE_GUI 
	if(showSettings)
		gui.draw();
#endif
}


bool EffectsSystem::windowResized(int w, int h) {
	window.x = w;
	window.y = h;
	return particleSystem.setWindowSize( Vec2f( w, h ) );
}


void EffectsSystem::keyPressed  (int key){ 
    switch(key) {
		case '1':
			fluidDrawer.setDrawMode(MSA::kFluidDrawColor);
			break;
			
		case '2':
			fluidDrawer.setDrawMode(MSA::kFluidDrawMotion);
			break;
			
		case '3':
			fluidDrawer.setDrawMode(MSA::kFluidDrawSpeed);
			break;
			
		case '4':
			fluidDrawer.setDrawMode(MSA::kFluidDrawVectors);
			break;
			
		case 'd':
			drawFluid ^= true;
			break;
			
		case 'p':
			drawParticles ^= true;
			break;
			
		case 'f':
			ofToggleFullscreen();
			break;
			
		case 'r':
			fluidSolver.reset();
			break;
			
		case 'b': {
			Timer timer;
			const int ITERS = 3000;
			timer.start();
			for( int i = 0; i < ITERS; ++i ) fluidSolver.update();
			timer.stop();
			cout << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
		}
			break;
			
    }
}

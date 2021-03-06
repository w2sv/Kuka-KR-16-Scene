#define _CRT_SECURE_NO_WARNINGS

#include "state.h"
#include "help.h"
#include "window.h"
#include "input.h"
#include "light.h"
#include "geometry.h"
#include "utils.h"
#include "robot.h"
#include "camera.h"
#include "image.h"
#include "text.h"
#include "skybox.h"
#include "platform.h"

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"



int main(int argc, char** argv) {
	init(argc, argv);
	
	// load objects
	Robot::loadObjects();
	Robot::setObjectMaterials();

	// load textures
	Robot::loadTextures();
	Skybox::loadTextures();

	// start main loop
	glutMainLoop();

	return 0;
}


void drawScene(const Robot& robot){
	Platform::draw();
	robot.draw();
	Skybox::draw();
}


void processInput(Robot& robot, Camera& camera) {

	/// ---------------------Robot----------------------

	// axes angle/velocity
	robot.update();

	// toggle tcp coord system
	if (cg_key::specialKeyState(GLUT_KEY_F1) == 1)
		robot.toggleTCPCoordSystem();

	// home position
	if (cg_key::specialKeyState(GLUT_KEY_F2) == 1) {

		// snap
		if (cg_key::specialKeyState(GLUT_KEY_SHIFT_L) != 0)
			robot.assumeHomePosition(true);

		// approach
		else
			robot.assumeHomePosition(false);
	}

	// arbitrary axis configuration
	else if (cg_key::specialKeyState(GLUT_KEY_F3) == 1) {

		// snap
		if (cg_key::specialKeyState(GLUT_KEY_SHIFT_L) != 0)
			robot.setArbitraryAxesConfiguration();
		
		// approach
		else
			robot.approachArbitraryAxisConfiguration();
	}

	// approach random configuration infinitely
	else if (cg_key::specialKeyState(GLUT_KEY_F4) == 1)
		robot.toggleInfiniteArbitraryAxisConfigurationApproachMode();

	else if (cg_key::specialKeyState(GLUT_KEY_F5) == 1)
		robot.resetVelocities();

	/// ---------------------Camera----------------------

	// reset
	else if (1 == cg_key::specialKeyState(GLUT_KEY_LEFT))
		camera.reset();

	// orbit mode
	else if (1 == cg_key::specialKeyState(GLUT_KEY_RIGHT))
		camera.toggleMode(Camera::Mode::Orbit);

	// tcp mode
	else if (1 == cg_key::specialKeyState(GLUT_KEY_UP))
		camera.toggleMode(Camera::Mode::TCP);

	// reverse tcp mode
	else if (1 == cg_key::specialKeyState(GLUT_KEY_DOWN))
		camera.toggleMode(Camera::Mode::ReverseTCP);


	/// ---------------------Generic----------------------

	// quit program
	else if (cg_key::keyState('q'))
		exit(0); // Escape -> Programm beenden

	// toggle help
	else if (1 == cg_key::keyState(27))
		GlobalState::displayHelp = !GlobalState::displayHelp;

	// full screen mode
	else if (1 == cg_key::keyState('p'))
		toggleFullScreenMode();

	/// ---------------------Display----------------------

	// toggle fps
	else if (1 == cg_key::keyState('b'))
		GlobalState::displayFps = !GlobalState::displayFps;

	// global coord system
	else if (1 == cg_key::keyState('n'))
		GlobalState::drawCoordSystem = !GlobalState::drawCoordSystem;

	// toggle axes states text
	else if (cg_key::keyState('m') == 1)
		robot.toggleAxesStatesDisplay();

	/// ---------------------Graphics----------------------

	// wireframe mode
	else if (1 == cg_key::keyState('y'))
		GlobalState::drawMode = (GlobalState::drawMode == GL_FILL) ? GL_LINE : GL_FILL; // Wireframe on/off

	// light mode
	else if (1 == cg_key::keyState('x'))
		GlobalState::lightMode = !GlobalState::lightMode;

	// backfaceculling
	else if (1 == cg_key::keyState('c'))
		GlobalState::cullMode = !GlobalState::cullMode;

	// shading mode
	else if (1 == cg_key::keyState('v'))
		GlobalState::shadingMode = GlobalState::shadingMode == GL_FLAT ? GL_SMOOTH : GL_FLAT;

	/// ---------------------Light----------------------

	// toggle tcp spotlight
	else if (1 == cg_key::keyState('i'))
		robot.toggleTCPSpotlight();

	// toggle tcp pointlight
	else if (1 == cg_key::keyState('o'))
		robot.toggleTCPPointlight();
}


void displayFunc(){
	static Robot robot;
	static Camera camera(&robot);
	static Text::OrthogonalProjection orthogonalProjection;

	GlobalState::updateFps();
	processInput(robot, camera);

	// Back-Buffer neu initialisieren
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Farbmodus oder Beleuchtungsmodus ?
	setDefaultLightAndMaterial(GlobalState::lightMode);

	// Kamera setzen (spherische Mausnavigation)
	camera.set();

	// Koordinatensystem zeichnen
	if (GlobalState::drawCoordSystem)
		drawCoordSystem(Platform::MEASUREMENTS, Platform::MEASUREMENTS, Platform::MEASUREMENTS);

	// Zeichenmodus einstellen (Wireframe on/off)
	glPolygonMode(GL_FRONT_AND_BACK, GlobalState::drawMode);

	// Backface Culling on/off);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	GlobalState::cullMode ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	// Set Shading Model
	glShadeModel(GlobalState::shadingMode);

	// Modell zeichnen
	drawScene(robot);

	// project text
	orthogonalProjection.activate(true);

	if (GlobalState::displayHelp)
		displayHelp();
	if (GlobalState::displayFps)
		displayFps();
	robot.displayText();

	orthogonalProjection.deactivate(true);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	glFlush();				// Daten an GL-Server (fuer die Darstellung) schicken
	glutSwapBuffers();		// Buffers wechseln
}
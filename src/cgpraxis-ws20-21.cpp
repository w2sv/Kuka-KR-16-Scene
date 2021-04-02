#define _CRT_SECURE_NO_WARNINGS

#include <iostream>  // !

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

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"


const Extrema groundMeasurements(-20, 20);


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


void drawPlatform() {
	Color(.1).render();
	drawQuadraticPlane(groundMeasurements, 150, -0.01);

	Color(.8, .0, .0).render();
	drawQuadraticGrid(groundMeasurements, 40, 0.01);
}


void drawScene(const Robot& robot){
	Skybox::draw();
	drawPlatform();
	robot.draw();
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
		if (cg_key::specialKeyState(GLUT_KEY_SHIFT_L) != 0)
			robot.assumeHomePosition(true);
		else
			robot.assumeHomePosition(false);
	}

	else if (cg_key::specialKeyState(GLUT_KEY_F3) == 1) {
		if (cg_key::specialKeyState(GLUT_KEY_SHIFT_L) != 0)
			robot.setArbitraryAxesConfiguration();
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

	else if (1 == cg_key::keyState('i'))
		robot.toggleTCPSpotlight();

	else if (1 == cg_key::keyState('o')) {
		GlobalState::sunlight = !GlobalState::sunlight;

		if (!GlobalState::sunlight)
			sunlight.disable();
	}
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
		drawCoordSystem(groundMeasurements, groundMeasurements, groundMeasurements);

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

	if (GlobalState::sunlight)
		drawSunlight();

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
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"

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




static Extrema groundMeasures = Extrema(-20, 20);
static Robot* robot = new Robot;



int main(int argc, char** argv) {
	init(argc, argv);

	// load objects
	Robot::loadObjects();
	Robot::setObjectMaterials();

	// load textures
	Robot::loadTextures();

	// start main loop
	glutMainLoop();

	return 0;
}



void drawScene(){
	setColor(.1, .1, .1);
	drawPlane(groundMeasures, groundMeasures);
	drawQuadraticGrid(groundMeasures, 40, Color(.8, .0, .0));

	robot->draw();
	robot->update();

	if (cg_key::specialKeyState(GLUT_KEY_F1) == 1)
		robot->reset();

	else if (cg_key::specialKeyState(GLUT_KEY_F2) == 1)
		robot->setArbitraryAxesConfiguration();
	
	else if (cg_key::specialKeyState(GLUT_KEY_F3) == 1)
		robot->toggleDrawTCPCoordSystem();
	
	else if (cg_key::specialKeyState(GLUT_KEY_F4) == 1)
		robot->toggleDisplayAxesStates();
	
	else if (cg_key::specialKeyState(GLUT_KEY_F5) == 1)
		robot->initializeArbitraryAxisConfigurationApproach();
	
	else if (cg_key::specialKeyState(GLUT_KEY_F6) == 1)
		robot->toggleInfiniteArbitraryAxisConfigurationApproachMode();
}



void displayFunc(){
	static cg_key key;
	static Camera camera(robot);
	static Text::OrthogonalProjection orthogonalProjection;

	GlobalState::updateFps();


	if (cg_key::keyState(27))
		exit(0); // Escape -> Programm beenden
	
	else if (1 == cg_key::keyState('F'))
		GlobalState::displayFps = !GlobalState::displayFps;	// Framecounter on/off
	
	else if (1 == cg_key::keyState('H'))
		GlobalState::displayHelp = !GlobalState::displayHelp;	// Hilfetext on/off
	
	else if (1 == cg_key::keyState('K'))
		GlobalState::drawCoordSystem = !GlobalState::drawCoordSystem;	// Koordinatensystem on/off
	
	else if (1 == cg_key::keyState('W'))
		GlobalState::drawMode = (GlobalState::drawMode == GL_FILL) ? GL_LINE : GL_FILL; // Wireframe on/off
	
	else if (1 == cg_key::keyState('L'))
		GlobalState::lightMode = !GlobalState::lightMode;	// Beleuchtung on/off
	
	else if (1 == cg_key::keyState('C'))
		GlobalState::cullMode = !GlobalState::cullMode; // Backfaceculling on/off

	// camera
	else if (1 == cg_key::specialKeyState(GLUT_KEY_LEFT))
		camera.reset();

	else if (1 == cg_key::keyState(GLUT_KEY_RIGHT))
		camera.toggleMode(Camera::Mode::Orbit);
	
	else if (1 == cg_key::specialKeyState(GLUT_KEY_UP))
		camera.toggleMode(Camera::Mode::TCP);

	else if (1 == cg_key::keyState(GLUT_KEY_DOWN))
		camera.toggleMode(Camera::Mode::ReverseTCP);

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
		drawCoordSystem(groundMeasures, groundMeasures, groundMeasures);

	// Zeichenmodus einstellen (Wireframe on/off)
	glPolygonMode(GL_FRONT_AND_BACK, GlobalState::drawMode);

	// Backface Culling on/off);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	GlobalState::cullMode ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	// Modell zeichnen
	drawScene();

	// Hilfetext einblenden
	if (GlobalState::displayHelp || GlobalState::displayFps || robot->textToBeDisplayed()) {
		orthogonalProjection.activate(true);

		if (GlobalState::displayHelp)
			displayHelp();
		if (GlobalState::displayFps)
			displayFps();
		if (robot->textToBeDisplayed())
			robot->displayText();

		orthogonalProjection.deactivate(true);
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	glFlush();				// Daten an GL-Server (fuer die Darstellung) schicken
	glutSwapBuffers();		// Buffers wechseln
}
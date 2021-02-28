#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include "Include/freeglut.h"
#include "Include/glext.h"

#include "help.h"
#include "window.h"
#include "input.h"
#include "light.h"
#include "geometry.h"
#include "utils.h"
#include "robot.h"
#include "camera.h"
#include "environment.h"
#include "image.h"



int main(int argc, char** argv) {
	init(argc, argv);

	// load objects
	Robot::loadObjects();
	Robot::setObjectMaterials();

	// start main loop
	glutMainLoop();

	return 0;
}


static Extrema groundMeasures = Extrema(-20, 20);


void drawScene(){
	setColor(.1, .1, .1);
	drawPlane(groundMeasures, groundMeasures);
	drawQuadraticGrid(groundMeasures, 40, Color(.8, .0, .0));

	static Robot* robot = new Robot();
	robot->draw();
	robot->update();

	if (cg_key::specialKeyState(GLUT_KEY_F1) == 1)
		robot->reset();
	else if (cg_key::specialKeyState(GLUT_KEY_F2) == 1)
		robot->setArbitraryAxesConfiguration();
	else if (cg_key::specialKeyState(GLUT_KEY_F3) == 1)
		robot->toggleDrawTCPCoordSystem();
	else if (cg_key::specialKeyState(GLUT_KEY_F4) == 1)
		robot->toggleDisplayAxesAngles();
}

void displayFunc()
{
	static cg_help help;
	static cg_key key;
	static Camera camera;

	// Tastatur abfragen
	// Achtung: einmaliges Betätigen funktioniert so nur mit glutIgnoreKeyRepeat(true) (siehe main())
	if (key.keyState(27)) {
		exit(0); // Escape -> Programm beenden
	}
	else if (1 == key.keyState('F')) {
		help.toggleFps();	// Framecounter on/off
	}
	else if (1 == key.keyState('H')) {
		help.toggle();	// Hilfetext on/off
	}
	else if (1 == key.keyState('K')) {
		cg_globState::drawCoordSystem = toggleFlag(cg_globState::drawCoordSystem);	// Koordinatensystem on/off
	}
	else if (1 == key.keyState('W')) {
		cg_globState::drawMode = (cg_globState::drawMode == GL_FILL) ? GL_LINE : GL_FILL; // Wireframe on/off
	}
	else if (1 == key.keyState('L')) {
		cg_globState::lightMode = !cg_globState::lightMode;	// Beleuchtung on/off
	}
	else if (1 == key.keyState('C')) {
		cg_globState::cullMode = !cg_globState::cullMode; // Backfaceculling on/off
	}
	//else if (1 == key.keyState('I')){
	//	globState.cameraHelper[0] = 0;	// Initialisierung der Kamera
	//	globState.cameraHelper[1] = 0;
	//}
	/*else if (key.keyState('T'))
		camera.setMode(Camera::Mode::TCP);
	else if (key.keyState('O'))
		camera.setMode(Camera::Mode::Observer);*/

	// Szene zeichnen: CLEAR, SETCAMERA, DRAW_SCENE

	// Back-Buffer neu initialisieren
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Farbmodus oder Beleuchtungsmodus ?
	setDefaultLightAndMaterial(cg_globState::lightMode);

	// Kamera setzen (spherische Mausnavigation)
	camera.set();

	// Koordinatensystem zeichnen
	if (cg_globState::drawCoordSystem)
		drawCoordSystem(groundMeasures, groundMeasures, groundMeasures);

	// Zeichenmodus einstellen (Wireframe on/off)
	glPolygonMode(GL_FRONT_AND_BACK, cg_globState::drawMode);

	// Backface Culling on/off);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	cg_globState::cullMode ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	// Modell zeichnen /////////////////!!!!!!!!!!!!!!!!!!!!!!!!///////////////////////
	drawScene();

	// Hilfetext einblenden
	help.draw();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	glFlush();				// Daten an GL-Server (fuer die Darstellung) schicken
	glutSwapBuffers();		// Buffers wechseln
}
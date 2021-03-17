#include "window.h"

#include "input.h"
#include "state.h"
#include "utils.h"

#include "../dependencies/freeglut.h"
#include "../dependencies/glext.h"


void init(int argc, char **argv){
	int MainWin;		// Identifier vom Hauptfenster
	int MainMenu;		// Identifier vom Hauptmenu

	// GLUT initialisieren
	glutInit(&argc, argv);

	// Fensterinitialisierung
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	glutInitWindowPosition(WIN_POS_X, WIN_POS_Y);
	glutInitDisplayMode(USED_MODUS);
	MainWin = glutCreateWindow(WINDOW_TITLE);

	// OpenGL Initialisierung
	glEnable(GL_DEPTH_TEST);	// Z-Buffer aktivieren

	glClearColor(0.5, 0, 1, 1);

	// Menue erzeugen
	MainMenu = glutCreateMenu(menuFunc);
	glutAddMenuEntry(MENU_TEXT_WIREFRAME, ID_MENU_WIREFRAME);
	glutAddMenuEntry(MENU_TEXT_SHADE, ID_MENU_SHADE);
	glutAddMenuEntry(MENU_TEXT_NO_NORMALS, ID_MENU_NO_NORMALS);
	glutAddMenuEntry(MENU_TEXT_PER_SURFACE_NORMALS, ID_MENU_PER_SURFACE_NORMALS);
	glutAddMenuEntry(MENU_TEXT_PER_VERTEX_NORMALS, ID_MENU_PER_VERTEX_NORMALS);
	glutAddMenuEntry(MENU_TEXT_EXIT, ID_MENU_EXIT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);		// Menue haengt an der rechten Maustaste

	// Callbackfunktionen binden
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MouseMove);
	glutPassiveMotionFunc(MouseMove);
	glutKeyboardFunc(KeyboardDownFunc);
	glutKeyboardUpFunc(KeyboardUpFunc);
	glutSpecialFunc(SpecialKeyboardFunc);
	glutSpecialUpFunc(SpecialKeyboardUpFunc);
	glutIdleFunc(idleFunc);
	glutIgnoreKeyRepeat(GL_TRUE);
}


/////////////////////////////////////////////////////////////////////////////////
//	CALLBACK Funktion fuer Idle: wird aufgerufen, Standard-Callback
/////////////////////////////////////////////////////////////////////////////////
void idleFunc(){
	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////
//	Resize CALLBACK Funktion - wird aufgerufen, wenn sich die
//	Fenstergroesse aendert
//		w,h: neue Breite und Hoehe des Fensters
/////////////////////////////////////////////////////////////////////////////////
void reshapeFunc(int w, int h){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / (double)h, 1, 2000);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);	// sicherheitshalber
	glLoadIdentity();				// Modelierungsmatrix einstellen
	GlobalState::screenSize[0] = w;
	GlobalState::screenSize[1] = h;
}


/////////////////////////////////////////////////////////////////////////////////
//	Menu CALLBACK Funktion
/////////////////////////////////////////////////////////////////////////////////
void menuFunc(int item){
	switch (item){
		case ID_MENU_EXIT:
			exit(0);
			break;
		case ID_MENU_WIREFRAME:
			GlobalState::drawMode = GlobalState::drawMode == GL_FILL ? GL_LINE : GL_FILL;
			break;
		case ID_MENU_SHADE:
			GlobalState::lightMode = GlobalState::lightMode == GL_TRUE ? GL_FALSE : GL_TRUE;
			break;
		case ID_MENU_NO_NORMALS:
			GlobalState::normalMode = 0;
			break;
		case ID_MENU_PER_SURFACE_NORMALS:
			GlobalState::normalMode = 1;
			break;
		case ID_MENU_PER_VERTEX_NORMALS:
			GlobalState::normalMode = 2;
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////////
//	Fullscreen toggling
/////////////////////////////////////////////////////////////////////////////////
void toggleFullScreenMode() {
	static Vector2 previousWindowSize;
	static Vector2 previousWindowPosition;

	if (previousWindowSize.isEmpty() && previousWindowPosition.isEmpty()) {
		previousWindowSize.set(GlobalState::screenSize[0], GlobalState::screenSize[1]);
		previousWindowPosition.set(glutGet(GLUT_WINDOW_X), glutGet(GLUT_WINDOW_Y));

		glutFullScreen();
	}
	else {
		glutPositionWindow(previousWindowPosition.x, previousWindowPosition.y);
		reshapeFunc(previousWindowSize.x, previousWindowSize.y);

		previousWindowSize.clear();
		previousWindowPosition.clear();
	}
}


/////////////////////////////////////////////////////////////////////////////////
//	FPS independent velocity
/////////////////////////////////////////////////////////////////////////////////
VelocityFpsRegularizer::VelocityFpsRegularizer(unsigned long microsecondsThreshold):
	microsecondsThreshold(microsecondsThreshold),
	lastTimeCheckpoint(std::chrono::steady_clock::now())
{}


bool VelocityFpsRegularizer::sufficientAmountOfTimePassed() {
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::microseconds>(now - lastTimeCheckpoint).count() > microsecondsThreshold) {
		lastTimeCheckpoint = now;
		return true;
	}
	return false;
}
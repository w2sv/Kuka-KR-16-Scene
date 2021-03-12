#include "input.h"

#include "../dependencies/freeglut.h"


char cg_key::keys[] = { 0 };
char cg_key::specialKeys[] = { 0 };
int cg_mouse::buttons[] = { 0 };
int cg_mouse::pos[] = { 0 };



/////////////////////////////////////////////////////////////////////////////////
//	Tastatur CALLBACK Funktion (Taste gedrückt)
//		key: Wert der Taste die gedrueckt wurde
//		x,y: Position des Mauskursors auf dem Viewport
/////////////////////////////////////////////////////////////////////////////////
void KeyboardDownFunc(unsigned char key, int x, int y)
{
	cg_key::keys[key] = 1;
}


/////////////////////////////////////////////////////////////////////////////////
//	Tastatur CALLBACK Funktion (Taste losgelassen)
//		key: Wert der Taste die losgelassen wurde
//		x,y: Position des Mauskursors auf dem Viewport
/////////////////////////////////////////////////////////////////////////////////
void KeyboardUpFunc(unsigned char key, int x, int y)
{
	cg_key::keys[key] = 0;
}


/////////////////////////////////////////////////////////////////////////////////
//	Tastatur CALLBACK Funktion
//		key: Wert der Taste die gedrueckt wurde
//		x,y: Position des Mauskursors auf dem Viewport
/////////////////////////////////////////////////////////////////////////////////
void SpecialKeyboardFunc(int key, int x, int y)
{
	cg_key::specialKeys[key > 99 ? key - 87 : key] = 1;
}


/////////////////////////////////////////////////////////////////////////////////
//	Tastatur CALLBACK Funktion
//		key: Wert der Taste die losgelassen wurde
//		x,y: Position des Mauskursors auf dem Viewport
/////////////////////////////////////////////////////////////////////////////////
void SpecialKeyboardUpFunc(int key, int x, int y)
{
	cg_key::specialKeys[key > 99 ? key - 87 : key] = 0;
}


/////////////////////////////////////////////////////////////////////////////////
//	Maus Button CALLBACK Funktion
//		button - Welche Taste betaetigt bzw. losgelassen wurde
//		state  - Status der State (GLUT_DOWN, GLUT_UP)
//		x, y   - Fensterkoordinaten des Mauszeigers
/////////////////////////////////////////////////////////////////////////////////
void MouseFunc(int button, int state, int x, int y)
{
	cg_mouse::pos[2] = cg_mouse::pos[0];
	cg_mouse::pos[3] = cg_mouse::pos[1];
	cg_mouse::pos[0] = x;
	cg_mouse::pos[1] = y;
	
	if (button >= GLUT_LEFT_BUTTON && button <= GLUT_RIGHT_BUTTON)
		cg_mouse::buttons[button] = GLUT_DOWN == state;

	// wheel event buttons finishing on GLUT_UP after being effectuated
	else if (button >= GLUT_MOUSE_WHEEL_UP && button <= GLUT_MOUSE_WHEEL_DOWN)
		cg_mouse::buttons[button] = GLUT_UP == state;

	glutPostRedisplay();
}


/////////////////////////////////////////////////////////////////////////////////
//	Maus Movement CALLBACK Funktion fuer GlutMotionFunc und GlutPassiveMotionFunc
/////////////////////////////////////////////////////////////////////////////////
void MouseMove(int x, int y)
{
	cg_mouse::pos[2] = cg_mouse::pos[0];
	cg_mouse::pos[3] = cg_mouse::pos[1];
	cg_mouse::pos[0] = x;
	cg_mouse::pos[1] = y;
	glutPostRedisplay();
}

char cg_key::keyState(unsigned char  k)
{
	if (1 == keys[k])
	{
		keys[k] = 2;
		return 1;
	}
	return keys[k];
}

char cg_key::specialKeyState(int  k)
{
	if (k > 99) 
		k -= 87;
	if (1 == specialKeys[k]){
		specialKeys[k] = 2;
		return 1;
	}
	return specialKeys[k];
}



int cg_mouse::buttonState(unsigned int b){
	if (b <= 2 && buttons[b] == 1){
		buttons[b] = 2;
		return 1;
	}

	// reset wheel event buttons on query whilst being set to true
	else if (b >= 3 && buttons[b]) {
		buttons[b] = 0;
		return 1;
	}
	return buttons[b];
}


int cg_mouse::moveX()
{
	return (pos[0] - pos[2]);
}


int cg_mouse::moveY()
{
	return (pos[1] - pos[3]);
}


int cg_mouse::posX()
{
	return pos[0];
}


int cg_mouse::posY()
{
	return pos[1];
}

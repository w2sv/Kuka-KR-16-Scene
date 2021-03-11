#ifndef INPUT_H
#define INPUT_H


#define GLUT_MOUSE_WHEEL_UP 3
#define GLUT_MOUSE_WHEEL_DOWN 4


// von GLUT benoetigte Callback-Funktionen
void KeyboardDownFunc(unsigned char key, int x, int y);	// Tastendruck
void KeyboardUpFunc(unsigned char key, int x, int y);	// Taste loslassen
void SpecialKeyboardFunc(int key, int x, int y);			// Sonderzeichen
void SpecialKeyboardUpFunc(int key, int x, int y);		// Sonderzeichen loslassen
void MouseFunc(int button, int state, int x, int y);		// Maustasten
void MouseMove(int x, int y);							// Mausbewegung



class cg_key
{
public:
	// Aktuellen Status der normalen Taste k zurueckgeben
	// Parameter k: ASCI-Code/char der Taste; ESC = 27
	// Rueckgabewerte: 0 = nicht gedueckt, 1 = gerade eben runtergedrueckt, 2 = gedrueckt gehalten
	static char keyState(unsigned char k);

	// Aktuellen Status der speziellen Taste k zurueckgeben
	// Parameter k ist eine der Konstanten:
	//	GLUT_KEY_F1 bis GLUT_KEY_F12
	//	GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN
	//	GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN
	//	GLUT_KEY_HOME, GLUT_KEY_BEGIN, GLUT_KEY_END
	//  GLUT_KEY_NUM_LOCK, GLUT_KEY_INSERT, GLUT_KEY_DELETE
	//  GLUT_KEY_SHIFT_L, GLUT_KEY_SHIFT_R
	//  GLUT_KEY_CTRL_L, GLUT_KEY_CTRL_R
	//  GLUT_KEY_ALT_L, GLUT_KEY_ALT_R
	// Rueckgabewerte: 0 = nicht gedueckt, 1 = gerade eben runtergedrueckt, 2 = gedrueckt gehalten
	static char specialKeyState(int k);

private:
	friend void KeyboardDownFunc(unsigned char key, int x, int y);
	friend void KeyboardUpFunc(unsigned char key, int x, int y);
	friend void SpecialKeyboardFunc(int key, int x, int y);
	friend void SpecialKeyboardUpFunc(int key, int x, int y);

	static char keys[256];
	static char specialKeys[31];
};


class cg_mouse
{
public:
	// Aktuellen Status der Mausbuttons zurueck geben
	// Parameter b ist eine der Konstanten: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
	// Rueckgabewerte: 0 = nicht gedueckt, 1 = gerade eben runtergedrueckt, 2 = gedrueckt gehalten
	static int buttonState(unsigned int b);
	
	// Aktuelle Bewegung der Maus auf der X-Achse zurueckgeben
	static int moveX();
	// Aktuelle Bewegung der Maus auf der Y-Achse zurueckgeben
	static int moveY();

	// X-Koordinate der aktuellen Mausposition zurueckgeben
	static int posX();
	// Y-Koordinate der aktuellen Mausposition zurueckgeben
	static int posY();

private:
	friend void MouseFunc(int button, int state, int x, int y);
	friend void MouseMove(int x, int y);

	static int buttons[5];	// 0 - links, 1 - mitte, 2 - rechts, 3 - mouse wheel up, 4 - mouse wheel down
	static int pos[4];	// 0,1 - aktuelle Position, 2,3 - letzte Position
};
#endif // INPUT_H

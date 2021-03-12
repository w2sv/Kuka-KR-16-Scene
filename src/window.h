#ifndef WINDOW_H
#define WINDOW_H


/////////////////////////////////////////////////////////////////////////////////
//	Fenster Initialisierung
/////////////////////////////////////////////////////////////////////////////////
//! Fenstertitel
#define WINDOW_TITLE "Praxisorientierte Einführung in die Computergraphik | WS 20-21 | Janek Zangenberg"

//! Die Startposition des Fensters (linke, obere Ecke)
#define WIN_POS_X	0
#define WIN_POS_Y	0

//! Breite des Fensters, Hoehe des Fensters
#define WIN_WIDTH	1280
#define WIN_HEIGHT	1024


/////////////////////////////////////////////////////////////////////////////////
//	OpenGL Darstellungsmodus
/////////////////////////////////////////////////////////////////////////////////
//! Default OpenGL Modus:  RGBA mit double Buffering und depth buffer (Z-Buffer)
//! Verwendung der Stencil und Accumulation Buffers, falls erforderlich
#if defined NEED_STENCIL && defined NEED_ACCUM
#define USED_MODUS	GLUT_RGBA |  GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL | GLUT_ACCUM
#elif defined NEED_STENCIL
#define USED_MODUS	GLUT_RGBA |  GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL
#elif defined NEED_ACCUM
#define USED_MODUS	GLUT_RGBA |  GLUT_DEPTH | GLUT_DOUBLE | GLUT_ACCUM
#else
#define USED_MODUS	GLUT_RGBA |  GLUT_DEPTH | GLUT_DOUBLE
#endif


/////////////////////////////////////////////////////////////////////////////////
//	Menuelemente des Kontextmenues (rechte Maustaste)
/////////////////////////////////////////////////////////////////////////////////
#define MENU_TEXT_WIREFRAME				"Wireframe on/off"
#define MENU_TEXT_SHADE					"Lighting/Shading"
#define MENU_TEXT_NO_NORMALS			"Lighting: set No Normals"
#define MENU_TEXT_PER_SURFACE_NORMALS 	"Lighting: set normals per surface"
#define MENU_TEXT_PER_VERTEX_NORMALS 	"Lighting: set normals per vertex"
#define MENU_TEXT_EXIT					"Exit"


enum MENU_IDs{
	ID_MENU_WIREFRAME = 1,
	ID_MENU_SHADE,
	ID_MENU_NO_NORMALS,
	ID_MENU_PER_SURFACE_NORMALS,
	ID_MENU_PER_VERTEX_NORMALS,
	ID_MENU_EXIT
};



/////////////////////////////////////////////////////////////////////////////////
//	CALLBACK Funktionen
/////////////////////////////////////////////////////////////////////////////////
void idleFunc ();					// Idle
void displayFunc ();				// GL-Displayfunktion
void reshapeFunc ( int, int );			// Neuzeichnen der Szene
void menuFunc ( int );					// Menue an Maustaste



/////////////////////////////////////////////////////////////////////////////////
//	Initialisierung
/////////////////////////////////////////////////////////////////////////////////
void init ( int argc, char **argv );


/////////////////////////////////////////////////////////////////////////////////
//	Fullscreen toggling
/////////////////////////////////////////////////////////////////////////////////
void toggleFullScreenMode();
#endif // WINDOW_H

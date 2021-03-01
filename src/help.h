#ifndef HELP_H
#define HELP_H

#include "utils.h"
#include "glutils.h"


// *** Help-Module, PrakCG Template

#ifdef __EXPORT_HELP

const char *spalte1[] =
{
	"Maus",
	"",
	"linke Taste      Kamerabewegung",
	"mittlere Taste   Zoom",
	"rechte Taste     Kontextmenü",

	NULL
};
const char *spalte2[] =
{
	"Tastatur:",
	"",
	"f,F    - Framerate (An/Aus)",
	"l,L    - Licht global (An/Aus)",
	"h,H,F1 - Hilfe (An/Aus)",
	"w,W    - WireFrame (An/Aus)",
	"k,K    - Koordinatensystem (An/Aus)",
	"", "", "", "",
	"ESC    - Beenden",

	NULL
};
#endif


void drawCoordSystem(Extrema& x, Extrema& y, Extrema& z, float coneScale = 1);


void printText(float x, float y, const char* text, void* font = GLUT_BITMAP_HELVETICA_18);
void printText(float x, float y, const char* text, float r, float g, float b, void* font = GLUT_BITMAP_HELVETICA_18);
void printTextShadow(float x, float y, const char* text, float r, float g, float b, float shadow, void* font = GLUT_BITMAP_HELVETICA_18);


void projectText(float x, float y, const char* text, Color& color, void* font = GLUT_BITMAP_HELVETICA_18);


class cg_help
{
public:
	void  toggle();
	void  toggleFps();
	void  setTitle(char *t);
	void  setWireframe(bool wf);
	bool  isWireframe();
	void  toggleKoordsystem();
	bool  isKoordsystem();
	float getFps();
	void  draw();

private:
	static bool  showhelp, showfps, wireframe, koordsystem;
	static int   frames;
	static float fps, bg_size;
	static const char  *title;
	void drawBackground();
	void printFps(float x, float y, void *font = GLUT_BITMAP_HELVETICA_18);
};


#endif	// HELP_H

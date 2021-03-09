#ifndef HELP_H
#define HELP_H

#include "utils.h"
#include "glutils.h"

#ifdef __EXPORT_HELP


const char *spalte1[] = {
	"Maus",
	"",
	"linke Taste      Kamerabewegung",
	"mittlere Taste   Zoom",
	"rechte Taste     Kontextmenü",

	NULL
};
const char *spalte2[] = {
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



namespace Text {
	static void* DEFAULT_FONT = GLUT_BITMAP_HELVETICA_18;

	void print(float x, float y, const char* text, void* font = DEFAULT_FONT);
	void print(float x, float y, const char* text, float r, float g, float b, void* font = DEFAULT_FONT);
	void printWithShadow(float x, float y, const char* text, float r, float g, float b, float shadow, void* font = DEFAULT_FONT);
	void project(float x, float y, const char* text, Color& color, void* font = DEFAULT_FONT);
}



class cg_help{
public:
	void draw();
	void displayFps();

private:
	static int frames;
	static float bg_size;

	void drawBackground();
};
#endif
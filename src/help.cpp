#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "state.h"

#include "help.h"



////////////////////////////////////////////////////////////
/// Text
////////////////////////////////////////////////////////////
void Text::display(float x, float y, const char* text, void* font){
	glRasterPos2f(x, y);
	for (size_t i = 0; i < strlen(text); ++i)
		glutBitmapCharacter(font, text[i]);
}



void Text::displayColored(float x, float y, const char* text, Color& color, void* font){
	color.render(false);
	display(x, y, text, font);
}



void Text::displayWithShadow(float x, float y, const char* text, Color& color, float shadow, void* font){
	displayColored(x + shadow, y - shadow, text, Color(Colors::BLACK), font);
	displayColored(x, y, text, color, font);
}



////////////////////////////////////////////////////////////
/// .OrthogonalProjection
////////////////////////////////////////////////////////////
Text::OrthogonalProjection::OrthogonalProjection():
	currentColor{NULL}
{}



void Text::OrthogonalProjection::activate(bool alterDepthTest) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glGetFloatv(GL_CURRENT_COLOR, currentColor);

	//orthogonale projektion setzen
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	if (alterDepthTest)
		glDisable(GL_DEPTH_TEST);
}



void Text::OrthogonalProjection::deactivate(bool alterDepthTest) {
	if (alterDepthTest)
		glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	// reset matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	glColor4fv(currentColor);
	glPopAttrib();

	// reset color
	memset(currentColor, NULL, 4);
}



////////////////////////////////////////////////////////////
/// Display functions
////////////////////////////////////////////////////////////
void drawBackground(){
	float CORNER_COORD = 0.8f;

	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.3f, 0.3f, 0.3f, 0.1f);
	glBegin(GL_QUADS);
		glVertex2f(-CORNER_COORD, CORNER_COORD);
		glVertex2f(-CORNER_COORD, -CORNER_COORD);
		glVertex2f(CORNER_COORD, -CORNER_COORD);
		glVertex2f(CORNER_COORD, CORNER_COORD);
	glEnd();
	glDisable(GL_BLEND);
}



void displayHelp() {
	static const char* spalte1[] = {
		"Maus",
		"",
		"linke Taste      Kamerabewegung",
		"mittlere Taste   Zoom",
		"rechte Taste     Kontextmenü",

		NULL
	};
	static const char* spalte2[] = {
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

	drawBackground();
	Text::displayWithShadow(-0.6f, 0.7f, "KUKA kr 16", Color(1, 1, 0), 0.003f, GLUT_BITMAP_TIMES_ROMAN_24);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float posy = 0.5f;
	int i = 0;
	while (spalte1[i]) {
		Text::display(-0.6f, posy, spalte1[i], GLUT_BITMAP_9_BY_15);
		posy -= 0.1f;
		++i;
	}

	posy = 0.5f;
	i = 0;
	while (spalte2[i]) {
		Text::display(0.05f, posy, spalte2[i], GLUT_BITMAP_9_BY_15);
		posy -= 0.1f;
		++i;
	}
}



void displayFps(){
	static char fpstext[9];

	sprintf(fpstext, "FPS: %.0f", GlobalState::fps);
	Text::displayWithShadow(-0.78f, -0.78f, fpstext, Color(1, 0.3, 0.1), 0.003f, GLUT_BITMAP_HELVETICA_18);	
}
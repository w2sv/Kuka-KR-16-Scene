#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "../dependencies/freeglut.h"

#define __EXPORT_HELP

#include "help.h"



////////////////////////////////////////////////////////////
/// Text
////////////////////////////////////////////////////////////
void Text::print(float x, float y, const char* text, void* font){
	glRasterPos2f(x, y);
	unsigned int l = strlen(text);
	for (unsigned int i = 0; i < l; ++i)
		glutBitmapCharacter(font, text[i]);
}



void Text::print(float x, float y, const char* text, float r, float g, float b, void* font){
	glColor3f(r, g, b);
	print(x, y, text, font);
}



void Text::printWithShadow(float x, float y, const char* text, float r, float g, float b, float shadow, void* font){
	print(x + shadow, y - shadow, text, 0, 0, 0, font);
	print(x, y, text, r, g, b, font);
}



void Text::project(float x, float y, const char* text, Color& color, void* font) {
	GLfloat akt_color[4];
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glGetFloatv(GL_CURRENT_COLOR, akt_color);

	//orthogonale projektion setzen
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_LIGHTING);

	print(x, y, text, color.r, color.g, color.b, font);

	glEnable(GL_LIGHTING);

	//reset matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glColor4fv(akt_color);
	glPopAttrib();
}



int cg_help::frames = 0;
float cg_help::bg_size = 0.8f;



void cg_help::drawBackground(){
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.3f, 0.3f, 0.3f, 0.1f);
	glBegin(GL_QUADS);
		glVertex2f(-bg_size, bg_size);
		glVertex2f(-bg_size, -bg_size);
		glVertex2f(bg_size, -bg_size);
		glVertex2f(bg_size, bg_size);
	glEnd();
	glDisable(GL_BLEND);
}



void cg_help::displayFps(){
	static time_t lastTime = 0;

	time_t now;
	time(&now);

	float fps = 0;

	//wenn ueber eine Sekunde vergangen ist
	if (now - lastTime >= 1){
		fps = ((float)frames) / (float)(now - lastTime);	//fps neu ausrechnen
		lastTime = now;	//alte Zeit speichern
		frames = 0;					//frame-zaehler zuruecksetzen
	}
	char fpstext[20];
	sprintf(fpstext, "FPS: %.0f", fps);
	Text::printWithShadow(-0.78f, -0.78f, fpstext, 1, 0.3, 0.1, 0.003f, GLUT_BITMAP_HELVETICA_18);
}



void cg_help::draw(){
	const static char* TITLE = "KUKA kr 16";

	GLfloat akt_color[4];
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glGetFloatv(GL_CURRENT_COLOR, akt_color);

	//orthogonale projektion setzen
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// hintergrund
	drawBackground();
	// title
	Text::printWithShadow(-0.6f, 0.7f, TITLE, 1.0f, 1.0f, 0.0f, 0.003f, GLUT_BITMAP_TIMES_ROMAN_24);
	// tasten
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	float posy = 0.5f;
	int i = 0;
	while (spalte1[i]){
		Text::print(-0.6f, posy, spalte1[i], GLUT_BITMAP_9_BY_15);
		posy -= 0.1f;
		++i;
	}
	posy = 0.5f;
	i = 0;
	while (spalte2[i]){
		Text::print(0.05f, posy, spalte2[i], GLUT_BITMAP_9_BY_15);
		posy -= 0.1f;
		++i;
	}

	// ruecksetzen
	glEnable(GL_DEPTH_TEST);
	//reset matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glColor4fv(akt_color);
	glPopAttrib();
}
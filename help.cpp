// *** Help-Module, PrakCG Template
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "Include/freeglut.h"
#define __EXPORT_HELP
#include "help.h"



bool cg_help::showhelp = false, cg_help::showfps = true, cg_help::wireframe = false, cg_help::koordsystem = false;
int cg_help::frames = 0;
float cg_help::fps = 0.0f, cg_help::bg_size = bg_size = 0.8f, cg_help::shadow = 0.003f;
const char * cg_help::title = "KUKA kr 16";


void  cg_help::toggle()
{
	showhelp = !showhelp;
}
void  cg_help::toggleFps()
{
	showfps = !showfps;
}
void  cg_help::setTitle(char *t)
{
	title = t;
}
void  cg_help::setWireframe(bool wf)
{
	wireframe = wf;
}
bool  cg_help::isWireframe()
{
	return wireframe;
}
void  cg_help::toggleKoordsystem()
{
	koordsystem = !koordsystem;
}
bool  cg_help::isKoordsystem()
{
	return koordsystem;
}
float cg_help::getFps()
{
	return fps;
}

void cg_help::drawBackground()
{
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.3f, 0.3f, 0.3f, 0.7f);
	glBegin(GL_QUADS);
		glVertex2f(-bg_size, bg_size);
		glVertex2f(-bg_size, -bg_size);
		glVertex2f(bg_size, -bg_size);
		glVertex2f(bg_size, bg_size);
	glEnd();
	glDisable(GL_BLEND);
}

/*
GLUT_BITMAP_8_BY_13
GLUT_BITMAP_9_BY_15
GLUT_BITMAP_TIMES_ROMAN_10
GLUT_BITMAP_TIMES_ROMAN_24
GLUT_BITMAP_HELVETICA_10
GLUT_BITMAP_HELVETICA_12
GLUT_BITMAP_HELVETICA_18
*/
void cg_help::printText(float x, float y, const char *text, void *font)
{
	glRasterPos2f(x, y);
	unsigned int l = strlen(text);
	for (unsigned int i = 0; i < l; ++i)
		glutBitmapCharacter(font, text[i]);
}

void cg_help::printText(float x, float y, const char *text, float r, float g, float b, void *font)
{
	glColor3f(r, g, b);
	printText(x, y, text, font);
}

void cg_help::printTextShadow(float x, float y, const char *text, float r, float g, float b, void *font)
{
	printText(x + shadow, y - shadow, text, 0, 0, 0, font);
	printText(x, y, text, r, g, b, font);
}

void cg_help::printFps(float x, float y, void *font)
{
	static time_t lastTime = 0;

	time_t now;
	time(&now);

	//wenn ueber eine Sekunde vergangen ist
	if (now - lastTime >= 1)
	{
		fps = ((float)frames) / (float)(now - lastTime);	//fps neu ausrechnen
		lastTime = now;	//alte Zeit speichern
		frames = 0;					//frame-zaehler zuruecksetzen
	}
	char fpstext[20];
	sprintf(fpstext, "FPS = %.1f", fps);
	printText(x, y, fpstext, font);
}

void cg_help::draw()
{
	++frames;
	if (!showhelp && !showfps) return;

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

	if (showhelp)
	{
		// hintergrund
		drawBackground();
		// title
		printTextShadow(-0.6f, 0.7f, title, 1.0f, 1.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24);
		// tasten
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		float posy = 0.5f;
		int i = 0;
		while (spalte1[i])
		{
			printText(-0.6f, posy, spalte1[i], GLUT_BITMAP_9_BY_15);
			posy -= 0.1f;
			++i;
		}
		posy = 0.5f;
		i = 0;
		while (spalte2[i])
		{
			printText(0.05f, posy, spalte2[i], GLUT_BITMAP_9_BY_15);
			posy -= 0.1f;
			++i;
		}
	}
	// fps
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	printFps(-0.78f, -0.78f);

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


void drawCoordSystem(Extrema& x, Extrema& y, Extrema& z, float coneScale){
	GLfloat i;
	GLfloat akt_color[4];
	GLint akt_mode;
	GLboolean cull_mode;

	glGetBooleanv(GL_CULL_FACE, &cull_mode);
	glDisable(GL_CULL_FACE);

	GLUquadricObj *spitze = gluNewQuadric();
	if (!spitze) 
		return; // quadric konnte nicht erzeugt werden

	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glGetFloatv(GL_CURRENT_COLOR, akt_color);
		glDisable(GL_LIGHTING);

		glBegin(GL_LINES);

			glColor3f(1.0, 0.0, 0.0);
			glVertex3f(x.min, 0, 0);
			glVertex3f(x.max, 0, 0);
			for (i = x.min; i <= x.max; i++)
			{
				glVertex3f(i, -0.15, 0.0);
				glVertex3f(i, 0.15, 0.0);
			}
			glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0, y.min, 0);
			glVertex3f(0, y.max, 0);

			for (i = y.min; i <= y.max; i++)
			{
				glVertex3f(-0.15, i, 0.0);
				glVertex3f(0.15, i, 0.0);
			}

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0, 0, z.min);
			glVertex3f(0, 0, z.max);

			for (i = z.min; i <= z.max; i++)
			{
				glVertex3f(-0.15, 0.0, i);
				glVertex3f(0.15, 0.0, i);
			}

		glEnd();

		// Ende Linienpaare
		glGetIntegerv(GL_MATRIX_MODE, &akt_mode);
		glMatrixMode(GL_MODELVIEW);

		// zuerst die X-Achse
		glPushMatrix();
			glTranslatef(x.max, 0., 0.);
			glScaleUniformly(coneScale);
			glRotatef(90., 0., 1., 0.);
			glColor3f(1.0, 0.0, 0.0);
			gluCylinder(spitze, 0.5, 0., 1., 10, 10);
		glPopMatrix();

		// dann die Y-Achse
		glPushMatrix();
			glTranslatef(0., y.max, 0.);
			glScaleUniformly(coneScale);
			glRotatef(-90., 1., 0., 0.);
			glColor3f(0.0, 1.0, 0.0);
			gluCylinder(spitze, 0.5, 0., 1., 10, 10);
		glPopMatrix();

		// zum Schluss die Z-Achse
		glPushMatrix();
			glTranslatef(0., 0., z.max);
			glScaleUniformly(coneScale);
			glColor3f(0.0, 0.0, 1.0);
			gluCylinder(spitze, 0.5, 0., 1., 10, 10);
		glPopMatrix();

		glMatrixMode(akt_mode);
		glColor4fv(akt_color);
	glPopAttrib();
	gluDeleteQuadric(spitze);

	if (!cull_mode) 
		glDisable(GL_CULL_FACE);
}

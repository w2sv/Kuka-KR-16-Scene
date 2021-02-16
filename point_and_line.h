#ifndef LINE_H
#define LINE_H

#include "Include/freeglut.h"

// Zeichnen eines Punkts an den Koordinaten s
// ohne Farbwerte
void drawPoint(GLfloat px, GLfloat py, GLfloat pz);
void drawPoint(GLfloat *p);

// mit Farbwerten
void drawPoint(GLfloat px, GLfloat py, GLfloat pz, GLfloat pr, GLfloat pg, GLfloat pb);
void drawPoint(GLfloat *p, GLfloat *c);

// Zeichnen einer Linie, vom Startpunkt s zum Endpunkt e
// ohne Farbwerte
void drawLine(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez);
void drawLine(GLfloat *s, GLfloat *e);

// mit Farbwerten
void drawLine(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez, GLfloat sr, GLfloat sg, GLfloat sb, GLfloat er, GLfloat eg, GLfloat eb);
void drawLine(GLfloat *s, GLfloat *e, GLfloat *sc, GLfloat *ec);
															  
#endif



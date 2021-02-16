#include "point_and_line.h"

// Zeichnen eines Punkts an den Koordinaten s
// ohne Farbwerte
void drawPoint(GLfloat px, GLfloat py, GLfloat pz) {
	glBegin(GL_POINTS);
		glVertex3f(px, py, pz);
	glEnd();
};
void drawPoint(GLfloat *p) {
	glBegin(GL_POINTS);
		glVertex3fv(p);
	glEnd();
};

// mit Farbwerten
void drawPoint(GLfloat px, GLfloat py, GLfloat pz, GLfloat pr, GLfloat pg, GLfloat pb) {
	glBegin(GL_POINTS);
		glColor3f(pr, pg, pb);
		glVertex3f(px, py, pz);
	glEnd();
};
void drawPoint(GLfloat *p, GLfloat *c) {
	glBegin(GL_POINTS);
		glColor3fv(c);
		glVertex3fv(p);
	glEnd();
};

// Hilfsfunktionen zum Zeichnen einer Linie, vom Startpunkt s zum Endpunkt e
// ohne Farbwerte
void drawLine(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat ex, GLfloat ey, GLfloat ez) {
	glBegin(GL_LINES);
		glVertex3f(sx, sy, sz);
		glVertex3f(ex, ey, ez);
	glEnd();
}

void drawLine(GLfloat *s, GLfloat *e) {
	glBegin(GL_LINES);
		glVertex3fv(s);
		glVertex3fv(e);
	glEnd();
}

// mit Farbwerten
void drawLine(GLfloat sx, GLfloat sy, GLfloat sz, GLfloat sr, GLfloat sg, GLfloat sb, GLfloat ex, GLfloat ey, GLfloat ez, GLfloat er, GLfloat eg, GLfloat eb) {
	glBegin(GL_LINES);
		glColor3f(sr, sg, sb);
		glVertex3f(sx, sy, sz);
		glColor3f(er, eg, eb);
		glVertex3f(ex, ey, ez);
	glEnd();
};

void drawLine(GLfloat *s, GLfloat *sc, GLfloat *e, GLfloat *ec) {
	glBegin(GL_LINES);
		glColor3fv(sc);
		glVertex3fv(s);
		glColor3fv(ec);
		glVertex3fv(e);
	glEnd();
};

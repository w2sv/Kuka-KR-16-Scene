#include "geometry.h"

#define SLICES 20
#define STACKS 20
#define LOOPS 1

void drawCube() {
	glPushMatrix();
		glutSolidCube(1.0);

		// Draht-Würfel Zeichnen
		glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
			setColor(0.1, 0.1, 0.1);
			glDepthFunc(GL_LEQUAL);
			glutWireCube(1);
		glPopAttrib();
	glPopMatrix();
};

void drawCuboid(float length, float height, float depth) {
	glPushMatrix();
	glScalef(length, height, depth);
	drawCube();
	glPopMatrix();
}

void drawQuadraticGrid(CoordinateExtrema& extrema, int tiles, Color& color) {
	color.renderMaterialized();

	glBegin(GL_LINES);
		for (float v = extrema.min; v <= extrema.max; v+= extrema.spread() / tiles) {
			v == extrema.min || v == extrema.max ? setColor(.6, .3, .3) : color.renderMaterialized();
			glVertex3f(v, 0, extrema.min);
			glVertex3f(v, 0, extrema.max);

			v == extrema.min || v == extrema.max ? setColor(.3, .3, .6) : color.renderMaterialized();
			glVertex3f(extrema.min, 0, v);
			glVertex3f(extrema.max, 0, v);
		};
	glEnd();
}

void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color) {
	const float Z_COORDINATE = -0.01;
	
	glBegin(GL_QUADS);
		// color.renderMaterialized();

		glVertex3f(xExtrema.min, Z_COORDINATE, yExtrema.min);
		glVertex3f(xExtrema.max, Z_COORDINATE, yExtrema.min);
		glVertex3f(xExtrema.max, Z_COORDINATE, yExtrema.max);
		glVertex3f(xExtrema.min, Z_COORDINATE, yExtrema.max);
	glEnd();
}

void drawCylinder(float startRadius, float endRadius, float height) {

	// Zeichnet einen Zylinder mit variablen Abmessungen ins WKS
	// Unterteilung: RINGS, SLICES
	// Lage: Basiskreis in X-Y-Ebene, Höhe entlang der +Z Achse

	GLUquadricObj* q = gluNewQuadric();

	glPushMatrix();
		// Mantelfläche
		gluCylinder(q, startRadius, endRadius, height, SLICES, STACKS);

		// Deckel
		glTranslatef(0, 0, height);
		gluDisk(q, 0, endRadius, SLICES, LOOPS);

		// Boden
		glTranslatef(0, 0, -height);
		glRotatef(180, 0, 1, 0);
		gluDisk(q, 0, startRadius, SLICES, LOOPS);
	glPopMatrix();

	gluDeleteQuadric(q);
};
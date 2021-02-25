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
	color.render();

	glBegin(GL_LINES);
		for (float v = extrema.min; v <= extrema.max; v+= extrema.spread() / tiles) {
			v == extrema.min || v == extrema.max ? setColor(.6, .3, .3) : color.render();
			glVertex3f(v, 0, extrema.min);
			glVertex3f(v, 0, extrema.max);

			v == extrema.min || v == extrema.max ? setColor(.3, .3, .6) : color.render();
			glVertex3f(extrema.min, 0, v);
			glVertex3f(extrema.max, 0, v);
		};
	glEnd();
}


void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color) {
	const float Z_COORDINATE = -0.01;
	
	glBegin(GL_QUADS);
		// color.render();

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


void drawOctagon(float heigth, float edgeLength, bool emphasizeEdges) {
	float z = heigth / 2;

	GLfloat octagonVertices[2][8][3] = {
		{
			// positive y's
			{-edgeLength, z, -edgeLength / 2}, // left |
			{-edgeLength / 2, z, -edgeLength}, // /
			{edgeLength / 2, z, -edgeLength}, // upper -
			{edgeLength, z, -edgeLength / 2}, // "\"

			// negative y's
			{edgeLength, z, edgeLength / 2}, // right |
			{edgeLength / 2, z, edgeLength}, // lower -
			{-edgeLength / 2, z, edgeLength}, // upside down "\"
			{-edgeLength, z, edgeLength / 2}, // upside down /
		},
		
		// negative z
		{
			// positive y's
			{-edgeLength, -z, -edgeLength / 2}, // left |
			{-edgeLength / 2, -z, -edgeLength}, // /
			{edgeLength / 2, -z, -edgeLength}, // upper -
			{edgeLength, -z, -edgeLength / 2}, // "\"

			// negative y's
			{edgeLength, -z, edgeLength / 2}, // right |
			{edgeLength / 2, -z, edgeLength}, // lower -
			{-edgeLength / 2, -z, edgeLength}, // upside down "\"
			{-edgeLength, -z, edgeLength / 2}, // upside down /
		}
	};

	// draw upper and lower plane
	for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
		glBegin(GL_POLYGON);
			for (size_t i = 0; i < 9; i++)
				glVertex3fv(octagonVertices[zPolarityIndex][i % 8]);
		glEnd();
	}
	
	// connect vertices opposing each other across xz-plane
	for (size_t faceIndex = 0; faceIndex < 7; faceIndex++) {
		glBegin(GL_POLYGON);
			for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
				for (size_t i = 0; i < 2; i++) {
					glVertex3fv(octagonVertices[zPolarityIndex][faceIndex + i + zPolarityIndex - (zPolarityIndex && i) * 2]);
				}
			}
		glEnd();
	}

	// draw octagon cage if desired
	if (emphasizeEdges)
		_drawOctagonEdges(octagonVertices);
}


void _drawOctagonEdges(GLfloat octagonVertices[2][8][3]) {
	glPushMatrix();
		EDGE_COLOR.render();

		glBegin(GL_LINES);
			for (size_t i = 0; i < 8; i++) {
				for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
					// draw edge along xz-plane
					glVertex3fv(octagonVertices[zPolarityIndex][i]);
					glVertex3fv(octagonVertices[zPolarityIndex][(i + 1) % 8]);

					// draw edge across xz-plane
					glVertex3fv(octagonVertices[0][(i + zPolarityIndex) % 8]);
					glVertex3fv(octagonVertices[1][(i + zPolarityIndex) % 8]);
				}
			}
		glEnd();
	glPopMatrix();
}
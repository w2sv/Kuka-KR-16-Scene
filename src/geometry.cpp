#include "geometry.h"


using namespace Axes;


#define SLICES 20
#define STACKS 20
#define LOOPS 1



void drawCube() {
	static const float COORD = 0.5;
	static const GLfloat VERTICES[2][4][3] = {
		{
			{COORD, COORD, COORD},
			{COORD, COORD, -COORD},
			{-COORD, COORD, -COORD},
			{-COORD, COORD, COORD}
		},
		{
			{COORD, -COORD, COORD},
			{ COORD, -COORD, -COORD },
			{ -COORD, -COORD, -COORD },
			{ -COORD, -COORD, COORD }
		}
	};

	static const GLfloat TEXTURE_COORDINATES[4][2] = {
			{1., 0.},
			{1., 1.},
			{0., 1.},
			{0., 0.}
	};

	glBegin(GL_QUADS);

	// lower face
	glNormal3f(0, 0, 1);
	for (size_t i = 0; i < 4; i++) {
		glTexCoord2fv(TEXTURE_COORDINATES[i]);
		glVertex3fv(VERTICES[0][i]);
	}

	// upper face
	glNormal3f(0, 0, -1);
	for (size_t i = 0; i < 4; i++) {
		glTexCoord2fv(TEXTURE_COORDINATES[i]);
		glVertex3fv(VERTICES[1][i]);
	}

	// front face
	glNormal3f(0, -1, 0);
	for (size_t i = 0; i < 2; i++)
		for (size_t j = 0; j < 2; j++) {
			glTexCoord2fv(TEXTURE_COORDINATES[i * 2 + j]);
			glVertex3fv(VERTICES[i][i != j]);
		}

	// back face
	glNormal3f(0, 1, 0);
	for (size_t i = 0; i < 2; i++)
		for (size_t j = 0; j < 2; j++) {
			glTexCoord2fv(TEXTURE_COORDINATES[i * 2 + j]);
			glVertex3fv(VERTICES[i][(i != j) + 2]);
		}

	// left face
	glNormal3f(-1, 0, 0);
	for (size_t i = 0; i < 2; i++)
		for (size_t j = 0; j < 2; j++) {
			glTexCoord2fv(TEXTURE_COORDINATES[i * 2 + j]);
			glVertex3fv(VERTICES[i][(i != j) + 1]);
		}

	// right face
	glNormal3f(1, 0, 0);
	for (size_t i = 0; i < 2; i++)
		for (size_t j = 0; j < 2; j++) {
			glTexCoord2fv(TEXTURE_COORDINATES[i * 2 + j]);
			glVertex3fv(VERTICES[i][(i != j) * 3]);
		}

	glEnd();
}


void indicateCurrentPosition() {
	glPushMatrix();
			Color(0., 1., 0.).render();
		glScalef(0.1, 0.1, 0.1);
		drawCube();
	glPopMatrix();
}


void drawQuadraticGrid(Extrema& extrema, int tiles, Color& color) {
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


void drawPlane(Extrema& xExtrema, Extrema& yExtrema) {
	const float Z_COORDINATE = -0.01;
	
	glBegin(GL_QUADS);
		glVertex3f(xExtrema.min, Z_COORDINATE, yExtrema.min);
		glVertex3f(xExtrema.max, Z_COORDINATE, yExtrema.min);
		glVertex3f(xExtrema.max, Z_COORDINATE, yExtrema.max);
		glVertex3f(xExtrema.min, Z_COORDINATE, yExtrema.max);
	glEnd();
}


void drawCylinder(float startRadius, float endRadius, float height) {

	GLUquadricObj* q = gluNewQuadric();

	gluQuadricNormals(q, true);
	gluQuadricTexture(q, true);

	glPushMatrix();
		X::rotate(270);

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


OctagonalPrismVertices drawOctagonalPrism(float heigth, float straightEdgeLength, float diagonalEdgeLength) {
	const float z = heigth / 2;
	const float lateralLength = (straightEdgeLength + diagonalEdgeLength) / 2;

	OctagonalPrismVertices octagonVertices = {
		{
			/* Subsequently denoted are the vertices in clockwise direction with respect
			to the respectively annotated edge;
			i.e. "left |" defines the lower vertex of the left | etc. */

			// positive y's
			{-lateralLength, z, -straightEdgeLength / 2}, //  left |
			{-straightEdgeLength / 2, z, -lateralLength}, // lower left "\"
			{straightEdgeLength / 2, z, -lateralLength}, // -
			{lateralLength, z, -straightEdgeLength / 2}, // lower right /

			// negative y's
			{lateralLength, z, straightEdgeLength / 2}, // right |
			{straightEdgeLength / 2, z, lateralLength}, // upper right "\"
			{-straightEdgeLength / 2, z, lateralLength}, // -
			{-lateralLength, z, straightEdgeLength / 2}, // upper left /
		},
		
		// negative z
		{
			// positive y's
			{-lateralLength, -z, -straightEdgeLength / 2}, //  left |
			{-straightEdgeLength / 2, -z, -lateralLength}, // lower left "\"
			{straightEdgeLength / 2, -z, -lateralLength}, // -
			{lateralLength, -z, -straightEdgeLength / 2}, // lower right /

			// negative y's
			{lateralLength, -z, straightEdgeLength / 2}, // right |
			{straightEdgeLength / 2, -z, lateralLength}, // upper right "\"
			{-straightEdgeLength / 2, -z, lateralLength}, // -
			{-lateralLength, -z, straightEdgeLength / 2}, // upper left /
		}
	};

	// draw upper and lower plane
	for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
		glBegin(GL_POLYGON);
			for (size_t i = 0; i < 9; i++)
				glVertex3fv(octagonVertices[zPolarityIndex][i % 8].data());
		glEnd();
	}
	
	// connect vertices opposing each other across xz-plane
	for (size_t faceIndex = 0; faceIndex < 8; faceIndex++) {
		glBegin(GL_POLYGON);
			for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
				for (size_t i = 0; i < 2; i++) {
					glVertex3fv(octagonVertices[zPolarityIndex][(faceIndex + i + zPolarityIndex - (zPolarityIndex && i) * 2) % 8].data());
				}
			}
		glEnd();
	}

	return octagonVertices;
}


void drawOctagonalPrismCage(OctagonalPrismVertices vertices) {
	glPushMatrix();
		glDepthFunc(GL_LEQUAL);
		glPushAttrib(GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);

		glBegin(GL_LINES);
			for (size_t i = 0; i < 8; i++) {
				for (size_t zPolarityIndex = 0; zPolarityIndex < 2; zPolarityIndex++) {
					// draw edge along xz-plane
					glVertex3fv(vertices[zPolarityIndex][i].data());
					glVertex3fv(vertices[zPolarityIndex][(i + 1) % 8].data());

					// draw edge across xz-plane
					glVertex3fv(vertices[0][(i + zPolarityIndex) % 8].data());
					glVertex3fv(vertices[1][(i + zPolarityIndex) % 8].data());
				}
			}
		glEnd();
		glPopAttrib();
	glPopMatrix();
}
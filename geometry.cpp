#include "geometry.h"


void drawCuboid(float length, float depth, float height, Color& color) {
	float xCoord = length / 2;
	float yCoord = depth / 2;
	float zCoord = height / 2;
	
	color.render();
	
	// bot
	glBegin(GL_QUADS);
		glVertex3f(-xCoord, -yCoord , -zCoord);
		glVertex3f(xCoord, -yCoord , -zCoord);
		glVertex3f(xCoord, yCoord , -zCoord);
		glVertex3f(-xCoord, yCoord , -zCoord);
	glEnd();

	// top
	glBegin(GL_QUADS);
		glVertex3f(-xCoord, -yCoord, zCoord);
		glVertex3f(-xCoord, yCoord, zCoord);
		glVertex3f(xCoord, yCoord, zCoord);
		glVertex3f(xCoord, -yCoord, zCoord);
	glEnd();

	// left
	glBegin(GL_QUADS);
		glVertex3f(xCoord, -yCoord, -zCoord);
		glVertex3f(xCoord, yCoord, -zCoord);
		glVertex3f(xCoord, yCoord, zCoord);
		glVertex3f(xCoord, -yCoord, zCoord);
	glEnd();

	// right
	glBegin(GL_QUADS);
		glVertex3f(-xCoord, -yCoord, -zCoord);
		glVertex3f(-xCoord, yCoord, -zCoord);
		glVertex3f(-xCoord, yCoord, zCoord);
		glVertex3f(-xCoord, -yCoord, zCoord);
	glEnd();

	// front
	glBegin(GL_QUADS);
		glVertex3f(-xCoord, -yCoord, -zCoord);
		glVertex3f(xCoord, -yCoord, -zCoord);
		glVertex3f(xCoord, -yCoord, zCoord);
		glVertex3f(-xCoord, -yCoord, zCoord);
	glEnd();

	// back
	glBegin(GL_QUADS);
		glVertex3f(-xCoord, yCoord, -zCoord);
		glVertex3f(xCoord, yCoord, -zCoord);
		glVertex3f(xCoord, yCoord, zCoord);
		glVertex3f(-xCoord, yCoord, zCoord);
	glEnd();
}

void drawGrid(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color) {
	glColor3f(.3, .3, .3);

	glBegin(GL_LINES);
	for (int i = -10; i <= 10; i++) {
		if (i == -10)
			glColor3f(.6, .3, .3);
		else
			glColor3f(.25, .25, .25);

		glVertex3f(i, 0, -10);
		glVertex3f(i, 0, 10);

		if (i == -10)
			glColor3f(.3, .3, .6);
		else
			glColor3f(.25, .25, .25);

		glVertex3f(-10, 0, i);
		glVertex3f(10, 0, i);
	};
	glEnd();
}

void drawPlane(CoordinateExtrema& xExtrema, CoordinateExtrema& yExtrema, Color& color) {
	const float Z_COORDINATE = -0.01;
	
	color.render();

	glBegin(GL_QUADS);
		glVertex3f(xExtrema.min, Z_COORDINATE, yExtrema.min);
		glVertex3f(xExtrema.max, Z_COORDINATE, yExtrema.min);
		glVertex3f(xExtrema.max, Z_COORDINATE, yExtrema.max);
		glVertex3f(xExtrema.min, Z_COORDINATE, yExtrema.max);
	glEnd();
}
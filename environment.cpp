#include "environment.h"


void drawBackgroundWalls(CoordinateExtrema groundMeasures) {
	static int tiles = 30;
	static Color color = Color(.2, .2, .2);

	glPushMatrix();
	glRotatep(90, Axes::X);
	glTranslatef(0, -groundMeasures.max, -groundMeasures.max);
	drawQuadraticGrid(groundMeasures, tiles, color);
	glPopMatrix();

	glPushMatrix();
	glRotatep(-90, Axes::Y);
	glTranslatef(-groundMeasures.max, groundMeasures.max, 0);
	drawQuadraticGrid(groundMeasures, tiles, color);
	glPopMatrix();
}
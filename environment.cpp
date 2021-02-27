#include "environment.h"


using namespace Axes;


void drawBackgroundWalls(Extrema groundMeasures) {
	static int tiles = 30;
	static Color color = Color(.2, .2, .2);

	glPushMatrix();
	X::rotate(90);
	glTranslatef(0, -groundMeasures.max, -groundMeasures.max);
	drawQuadraticGrid(groundMeasures, tiles, color);
	glPopMatrix();

	glPushMatrix();
	Y::rotate(-90);
	glTranslatef(-groundMeasures.max, groundMeasures.max, 0);
	drawQuadraticGrid(groundMeasures, tiles, color);
	glPopMatrix();
}
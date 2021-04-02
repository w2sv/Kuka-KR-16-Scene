#include "platform.h"

#include "glutils.h"
#include "geometry.h"
#include "light.h"

#include "../dependencies/freeglut.h"


void Platform::draw() {
	Material(
		RGBAParameter{1, 1, 1, 1},
		RGBAParameter{ 0.7, 0.7, 0.7, 1.0 },
		RGBAParameter{ 0.7, 0.7, 0.7, 1.0 },
		32.f,
		RGBAParameter{ 0.0, 0.0, 0.0, 1.0 }
	).set();

	Color(.1).render();
	drawSquarePlane(MEASUREMENTS, 150);

	glPushMatrix();
		glTransformationAxes::Z::translate(0.02);
		Color(.8, .0, .0).render();
		drawSquareGrid(MEASUREMENTS, 40);
	glPopMatrix();
}
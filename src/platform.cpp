#include "platform.h"

#include "glutils.h"
#include "geometry.h"


void Platform::draw() {
	Color(.1).render();
	drawQuadraticPlane(MEASUREMENTS, 150, -0.01);

	Color(.8, .0, .0).render();
	drawQuadraticGrid(MEASUREMENTS, 40, 0.01);
}
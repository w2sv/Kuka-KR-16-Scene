#pragma once

#include "Include/freeglut.h"

#include "utils.h"
#include "geometry.h"

class Robot
{
public:
	Robot();

	void moveArm(Vector3& position);
	const void draw();

private:
	const float PEDASTEL_CEILING_Z_COORDINATE = 2.;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.2;

	const void drawPedestal();

	const void drawArm();
	const void drawLowerSteelCylinder();
	const void drawLowerJoint();
};


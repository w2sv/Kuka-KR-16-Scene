#pragma once

#include "Include/freeglut.h"

#include "utils.h"
#include "geometry.h"

class Robot
{
public:
	Robot();

	void moveArm(Vector3 position);
	void draw();

private:
	void drawPedestal();
	void drawJoint();
};


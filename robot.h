#pragma once

#include <iostream>
#include <limits>

#include "Include/freeglut.h"

#include "utils.h"
#include "geometry.h"
#include "input.h"

struct OrientationDimension {
	public:
		float angle;
		
		OrientationDimension() {}  // TODO: abort respective need
		OrientationDimension(char incrementationKey, char decrementationKey, float angleLimit);
		void update();
	private:
		static cg_key* key;

		float angleLimit;

		char incrementationKey;
		char decrementationKey;

		void clipAngle();
};

struct Axis {
	OrientationDimension roll;
	OrientationDimension tilt;

	Axis(OrientationDimension& roll, OrientationDimension& tilt);
	void update();
};

class Robot
{
public:
	Robot();

	void moveArm(Vector3& position);
	const void draw();

private:
	Axis lowerAxis = Axis(OrientationDimension('a', 'd', 360), OrientationDimension('s', 'w', 45));

	const float PEDASTEL_CEILING_Z_COORDINATE = 2.;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.2;

	const void drawPedestal();

	const void drawArm();
	const void drawLowerSteelCylinder();
	const void drawLowerAxis();
};


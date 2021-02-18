#pragma once

#include <iostream>
#include <limits>
#include <functional>
#include <map>
#include <iterator>
#include <vector>

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
public:
	OrientationDimension roll, tilt;
	float height, width, depth;

	Axis() {}  // TODO: abort respective need
	Axis(OrientationDimension& roll, OrientationDimension& tilt, Measurements& measurements);
	void update();
	const void adjustMatrixOrientationAccordingly();

private:
	float centerHeight;
};

class Robot
{
public:
	Robot();

	void moveArm(Vector3& position);
	const void draw();
	void update();

private:
	const float PEDASTEL_CEILING_Z_COORDINATE = 2.;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.2;
	const float LOWER_AXIS_HEIGHT = 3.;

	Color BASE_COLOR = Color(230, 80, 21);

	Axis lowerAxis;
	Axis centralAxis;
	Axis outerAxis;

	std::vector<Axis*> axes;

	const void drawPedestal();
	const void drawLowerSteelCylinder();
	
	const void drawLowerAxis();
	const void drawCentralAxis();
	const void drawOuterAxis();

	std::map<Axis*, std::function<const void()>> axisDrawFunction;
};


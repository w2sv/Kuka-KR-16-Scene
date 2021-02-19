#pragma once

#include <iostream>
#include <limits>
#include <functional>
#include <map>
#include <vector>
#include <cmath>

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
		bool isFullRangeOfMotionDim;

		char incrementationKey;
		char decrementationKey;

		void clipAngle();
};

struct Axis {
public:
	OrientationDimension roll, tilt;
	float height, width, depth;
	float centerHeight;

	Axis() {}  // TODO: abort respective need
	Axis(OrientationDimension& roll, OrientationDimension& tilt, Measurements& measurements);
	void update();
	const void adjustMatrixOrientationAccordingly();
	void reset();
};

class Robot
{
public:
	static GLdouble* tcp_location_matrix;

	Robot();

	void moveArm(Vector3& position);
	const void draw();
	void update();
	void reset();

private:
	const float PEDASTEL_HEIGHT = 2.;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.2;

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


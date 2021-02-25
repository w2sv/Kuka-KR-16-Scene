#pragma once

#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>
#include <limits>
#include <functional>
#include <map>
#include <vector>
#include <cmath>
#include <sstream>
#include <string>

#include "Include/freeglut.h"

#include "utils.h"
#include "geometry.h"
#include "input.h"
#include "light.h"
#include "wavefront.h"


struct OrientationDimension {
	public:
		float angle;
		
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
	float height, width, length;
	float centerHeight;

	Axis(OrientationDimension&& roll, OrientationDimension&& tilt, Measurements&& measurements);
	void update();
	void adjustModelMatrixOrientationAccordingly() const;
	void reset();
};


class Robot
{
public:
	static void loadObjects();
	static void setObjectMaterials();

	Robot();

	void draw();  // TODO: make const again
	void moveArm(Vector3& position);
	void update();
	void reset();
	 
private:
	const static Color BASE_COLOR;
	void drawScrewHead() const;

	// ------------OBJECTS-----------------

	const static int N_OBJECTS = 1;
	static cg_object3D objects[N_OBJECTS];

	enum Object {
		HollowCylinder,
	};

	// ------------PARTS-------------------

	void drawPedestal() const;
	const float PEDASTEL_HEIGHT = 3.;

	void drawLowerSteelCylinder() const;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.2;

		// --------------AXES------------------

	std::vector<Axis*> axes;
	std::map<Axis*, std::function<const void()>> axis2DrawFunction;


	void drawLowerAxis() const;
	Axis lowerAxis;

	void drawCentralAxis() const;
	Axis centralAxis;

	void drawOuterAxis() const;
	Axis outerAxis;
};
#endif
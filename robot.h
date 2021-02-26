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


#pragma region Axis
struct Axis {
	OrientationDimension orientation;

	Axis(OrientationDimension&& orientation, float startAngle);
	void update();
	virtual void adjustModelMatrixOrientationAccordingly() const = 0;
	void reset();
};


struct RotationAxis : public Axis {
	using Axis::Axis;

	void adjustModelMatrixOrientationAccordingly() const;
};


struct TiltAxis : public Axis {
	using Axis::Axis;

	void adjustModelMatrixOrientationAccordingly() const;
};
#pragma endregion



class Robot
{
public:
	static void loadObjects();
	static void setObjectMaterials();

	Robot();

	void draw();  // TODO: make const again
	void update();
	void reset();
	 
private:
	const static Color BASE_COLOR;

	/* ------------OBJECTS----------------- */

	const static int N_OBJECTS = 2;
	static cg_object3D objects[N_OBJECTS];

	enum Object {
		HollowCylinder,
		ScrewHead,
	};

	/* -------------PARTS------------------ */

	void drawScrewHead() const;
	void drawAxisWeight() const;  // TODO: rename

	/* ------------COMPONENTS------------------- */

	void drawPedestal() const;
	const float PEDASTEL_HEIGHT = 3.;

	void drawLowerSteelCylinder() const;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.6;

		/* --------------AXES------------------ */

	std::vector<Axis*> axes;
	std::map<Axis*, std::function<const void()>> axis2DrawFunction;


	void drawLowerAxis() const;
	RotationAxis lowerAxis;

	void drawCentralAxis() const;
	// TiltAxis centralAxis;

	void drawOuterAxis() const;
	// RotationAxis outerAxis;
};
#endif
#pragma once

#ifndef ROBOT_H
#define ROBOT_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>  // !
#include <limits>
#include <functional>
#include <map>
#include <vector>
#include <random>

#include "Include/freeglut.h"

#include "geometry.h"
#include "input.h"
#include "wavefront.h"
#include "glutils.h"
#include "utils.h"


struct OrientationDimension {
	public:
		OrientationDimension(char incrementationKey, char decrementationKey, float startAngle, Extrema&& angleLimits);
		void update();
		void reset();
		float getAngle() const;
		void setArbitraryAngle();
	private:
		float angle;

		const float startAngle;
		const Extrema angleLimits;
		const bool isFullRangeOfMotionDim;
		const char incrementationKey, decrementationKey;

		void clipAngle();
};


#pragma region Axis
struct Axis {
	OrientationDimension orientation;

	Axis(OrientationDimension&& orientation);
	virtual void adjustModelMatrixOrientationAccordingly() const = 0;
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


std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints);



class Robot
{
public:
	static void loadObjects();
	static void setObjectMaterials();

	Robot();

	void draw();  // TODO: make const again
	void update();
	void reset();
	void setArbitraryAxesConfiguration();
	 
private:
	const static Color BASE_COLOR;

	/* ------------OBJECTS----------------- */

	const static int N_OBJECTS = 4;
	static cg_object3D objects[N_OBJECTS];

	enum Object {
		RotationAxis1,
		ScrewHead,
		TiltAxis1,
		TiltAxis2
	};

	/* -------------PARTS------------------ */

	const float AXIS_WEIGHT_HEIGTH = 1.15;
	void drawAxisWeight() const;

	static const std::vector<Vector2> SCREW_POSITIONS;
	void drawScrewCircle() const;

	/* ------------COMPONENTS------------------- */

	void drawPedestal() const;
	const float PEDASTEL_HEIGHT = 3.;

	void drawLowerSteelCylinder() const;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.6;

		/* --------------AXES------------------ */

	std::vector<Axis*> axes;
	std::map<Axis*, std::function<const void()>> axis2DrawFunction;


	void drawFirstAxis() const;
	RotationAxis firstAxis;

	void drawSecondAxis() const;
	TiltAxis secondAxis;

	void drawThirdAxis() const;
	TiltAxis thirdAxis;

	void drawFourthAxis() const;
	RotationAxis fourthAxis;
};
#endif
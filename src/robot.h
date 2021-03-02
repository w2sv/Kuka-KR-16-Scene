#pragma once

#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>  // !
#include <limits>
#include <functional>
#include <map>
#include <vector>
#include <random>
#include <sstream>
#include <string>
#include <math.h>

#include "../dependencies/freeglut.h"

#include "geometry.h"
#include "input.h"
#include "wavefront.h"
#include "glutils.h"
#include "utils.h"
#include "help.h"
#include "image.h"
#include "window.h"


void loadTextures();


#pragma region ParameterState
struct AxisParameterState {
public:
	AxisParameterState(float startValue, Extrema&& limits);

	void reset();
	void updateLimitReached();
	bool limitReached() const;
	void clipValue();
	float getValue() const;

	void operator+=(float increment);
	void operator-=(float decrement);
	bool operator<=(float value);
	bool operator>=(float value);
protected:
	float value;
	bool limitReached_b;

	const float startValue;
	const Extrema limits;
};


struct AngleState : public AxisParameterState {
	const char incrementationKey, decrementationKey;

	AngleState(float startValue, Extrema&& limits, char incrementationKey, char decrementationKey);
	void setArbitrarily();
};
struct VelocityState: public AxisParameterState {
	const char identificationKey;

	VelocityState(float max, char identificationKey);
};
#pragma endregion



struct OrientationDimension {
	public:
		OrientationDimension(AngleState&& angle, VelocityState&& velocity);

		void reset();
		void update();

		AngleState angle;
		VelocityState velocity;
	protected:
		void updatePosition();
		void updateVelocity();

		virtual void adjustAngle() = 0;
};


struct UnlimitedMotionOrientationDimension : public OrientationDimension {
	using OrientationDimension::OrientationDimension;

private:
	void adjustAngle();
};

struct LimitedMotionOrientationDimension : public OrientationDimension {
	using OrientationDimension::OrientationDimension;

private:
	void adjustAngle();
};


#pragma region Axis
struct Axis {
	OrientationDimension* orientation;

	Axis(OrientationDimension* orientation);
	~Axis();

	void adjustModelMatrixOrientationAccordingly() const;
	void adjustModelMatrixOrientationInversely() const;
};

struct RotationAxis : public Axis {
	RotationAxis(AngleState&& angle, VelocityState&& velocity);
};
struct TiltAxis : public Axis {
	TiltAxis(AngleState&& angle, VelocityState&& velocity);
};
#pragma endregion


std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints);



class Robot
{
public:
	static void loadObjects();
	static void setObjectMaterials();

	Robot();
	~Robot();

	void draw();  // TODO: make const again
	void update();
	void reset();
	void setArbitraryAxesConfiguration();

	void toggleDrawTCPCoordSystem();
	void toggleDisplayAxesAngles();
	void assumeSpatialTCPConfiguration() const;
private:
	const static Color BASE_COLOR;
	const int N_AXES = 4;

	bool drawTCPCoordSystem;
	bool displayAxesAngles_b;

	void displayAxesStates() const;

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

	void drawAxisWeight() const;

	static const std::vector<Vector2> SCREW_CIRCLE_POSITIONS;
	void drawScrewCircle() const;

	/* ------------COMPONENTS------------------- */

	void drawPedestal() const;
	const float PEDASTEL_HEIGHT = 3.;

	void drawBase() const;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.6;

		/* --------------AXES------------------ */

	std::vector<Axis*> axes;
	std::map<Axis*, std::function<const void()>> axis2DrawFunction;

	void drawFirstAxis() const;
	void drawSecondAxis() const;
	void drawThirdAxis() const;
	void drawFourthAxis() const;
};
#endif
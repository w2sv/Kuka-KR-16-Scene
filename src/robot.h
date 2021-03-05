#pragma once

#ifndef ROBOT_H
#define ROBOT_H

#include <iostream>  // !
#include <limits>
#include <functional>
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <math.h>
#include <stdexcept>

#include "../dependencies/freeglut.h"

#include "geometry.h"
#include "input.h"
#include "wavefront.h"
#include "glutils.h"
#include "utils.h"
#include "help.h"
#include "image.h"
#include "window.h"


#pragma region AxisParameterState
/*
*	Interface for structs encapsulating singular robot axis parameter modifyable by
*	user at runtime and the entirety of inherent attributes
*/
struct AxisParameterState {
public:
	AxisParameterState(float startValue, Extrema&& limits);

	float getValue() const;
	bool limitReached() const;
	void reset();

	void updateLimitReached();
	void clipValue();

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
	AngleState(float startValue, Extrema&& limits, char incrementationKey, char decrementationKey);
	const char incrementationKey, decrementationKey;
	
	/// Samples hardware randomized value within value limits
	int drawArbitraryValue() const;
	void setArbitrarily();
};
struct VelocityState: public AxisParameterState {
	VelocityState(float max, char identificationKey);
	const char identificationKey;
};
#pragma endregion



#pragma region OrientationDimension
struct OrientationDimension {
	public:
		OrientationDimension(AngleState&& angle, VelocityState&& velocity);

		AngleState angle;
		VelocityState velocity;

		void update();
		void reset();

		// ---------------TargetAngleAttributes-------------------

		enum TargetAngleState {
			Disabled,
			YetToBeReached,
			Reached
		};

		void setTargetAngleParameters();
		TargetAngleState getTargetAngleState() const;
		void resetTargetAngleParameters();
		int getTargetAngle() const;
	protected:
		void updateVelocity();
		void updateAngle();
		/// Adjusts angle state wrt specific OrientationDimension kind, i.e. subclass
		/// post update
		virtual void adjustAngle() = 0;

		// ---------------TargetAngleAttributes-------------------

		int targetAngle;
		bool targetAngleApproachManner;  // 0=decrementally, 1=incrementally
		TargetAngleState targetAngleState;

		/// Determines targetAngleApproachManner granting the quickest attainment
		virtual void determineTargetAngleApproachManner() = 0;
		void approachTargetAngle();
};


/**
* Orientation Dimension of unlimited range of motion
*/
struct UnlimitedOrientationDimension : public OrientationDimension {
	using OrientationDimension::OrientationDimension;

private:
	void adjustAngle();
	void determineTargetAngleApproachManner();
};



/**
* Orientation Dimension of limited range of motion
*/
struct LimitedOrientationDimension : public OrientationDimension {
	using OrientationDimension::OrientationDimension;

private:
	void adjustAngle();
	void determineTargetAngleApproachManner();
};
#pragma endregion



#pragma region Axis
struct Axis {
	OrientationDimension* orientation;

	Axis(OrientationDimension* orientation);
	~Axis();

	void adjustGLModelMatrixAccordingly() const;
	void adjustGLModelMatrixInversely() const;
	
	void adjustGLModelMatrixTargetAngleAccordingly() const;
};

struct YawAxis : public Axis {
	YawAxis(AngleState&& angle, VelocityState&& velocity);
};
struct TiltAxis : public Axis {
	TiltAxis(AngleState&& angle, VelocityState&& velocity);
};
#pragma endregion


std::vector<Vector2> discrete2DCircleRadiusPoints(float radius, int nPoints);


class Robot{
public:
	static void loadObjects();
	static void setObjectMaterials();

	static void loadTextures();

	Robot();
	~Robot();

	void draw();  // TODO: make const again
	void update();
	void reset();
	void setArbitraryAxesConfiguration();

	void initializeArbitraryAxisConfigurationApproach();

	void toggleDrawTCPCoordSystem();
	void toggleDisplayAxesStates();
	void toggleApproachArbitraryAxisConfigurationInfinitelyMode();

	void assumeSpatialTCPConfiguration() const;
private:
	const static Color BASE_COLOR;

	bool approachArbitraryAxisConfigurationInfinitely_b;
	bool approachArbitraryAxisConfiguration_b;

	bool drawTCPCoordSystem_b;
	bool displayAxesStates_b;

	void displayAxesStates() const;
	static void drawShrunkCoordSystem();
	void drawTargetAxesConfigurationCoordSystem() const;

	/* ------------Textures----------------- */

	static const int N_TEXTURES = 2;
	static cg_image textures[N_TEXTURES];

	enum Texture {
		Knobs,
		Steel
	};

	/* ------------Objects----------------- */

	const static int N_OBJECTS = 5;
	static cg_object3D objects[N_OBJECTS];

	enum Object {
		YawAxis1,
		ScrewHead,
		TiltAxis1,
		TiltAxis2,
		KukaLogo
	};

	/* -------------RepeatedlyUsedParts------------------ */

	void drawAxisWeight() const;
	void drawScrewCircle() const;
	static const std::vector<Vector2> SCREW_CIRCLE_POSITIONS;

	/* ------------------Components------------------- */

	void drawPedestal() const;
	const float PEDASTEL_HEIGHT = 3;

	void drawBottom() const;
	const float LOWER_STEEL_CYLINDER_HEIGHT = 1.6;

		/* --------------AXES------------------ */

	static const int N_AXES = 4;
	const static ModelviewMatrixTransformation relativeAxesStartConfigurationTransformations[N_AXES + 1];
	
	std::vector<Axis*> axes;
	static const std::vector<Vector3> AXIS_END_POSITION_ATTAINMENT_SHIFT_VECTORS;
	std::map<Axis*, std::function<const void()>> axis2DrawFunction;

	void drawFirstAxis() const;
	void drawSecondAxis() const;
	void drawThirdAxis() const;
	void drawFourthAxis() const;
};
#endif
#pragma once

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>

#include "../dependencies/freeglut.h"

#include "input.h"
#include "window.h"
#include "robot.h"
#include "glutils.h"


class Camera
{
public:
	Camera(Robot* robot);
	
	const void set();
	void reset();
	void toggleMode();
	void toggleGyrateMode();
private:
	enum Mode {
		Observer,
		Walk,
		TCP,
		Gyrate
	};

	const Vector2 START_POSITION = Vector2(0, cg_globState::screenSize[1] * 0.5);
	const float START_RADIUS = 40;
	const float RADIUS_MIN = 7.4;

	Vector2 position;
	Mode mode;
	float radius;

	Robot* robot;

	void setObserverMode();
	void setWalkMode();
	void setTCPMode();
	void setGyrateMode();

	void setCameraParameterAccordingly();

	void updateRadiusViaScroll();
	void updateRadiusViaWheelPressing();
	void floorRadius();
};
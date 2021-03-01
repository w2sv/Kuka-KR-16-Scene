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
		TCP,
		Gyrate
	};

	const Vector2 START_POSITION = Vector2(0, cg_globState::screenSize[1] / 0.75);
	const float START_RADIUS = 40;

	Robot* robot;

	Vector2 position;
	Mode mode;

	float radius;

	void setObserverMode();
	void setTCPMode();
	void setGyrateMode();

	void updateRadiusViaScroll();
	void setCameraParameterAccordingly();
};
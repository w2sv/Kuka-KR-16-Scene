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


class Camera{
public:
	Camera(Robot* robot);
	
	const void set();
	void reset();

	enum Mode {
		Observer,
		TCP,
		ReverseTCP,
		Orbit
	};

	void toggleMode(Mode mode);
private:
	const Vector2 START_POSITION;
	static const float START_RADIUS;
	static const float RADIUS_MIN;

	Vector2 position;
	Mode mode;
	float radius;

	Robot* robot;

	void setObserverMode();
	void setTCPMode();
	void setReverseTCPMode();
	void setOrbitMode();

	void setCameraParameterAccordingly();

	void updateRadiusViaScroll();
	void updateRadiusViaWheelPressing();
	void floorRadius();
};
#pragma once

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>

#include "Include/freeglut.h"

#include "input.h"
#include "window.h"
#include "robot.h"


class Camera
{
public:
	enum Mode {
		Observer,
		TCP
	};

	Camera();
	const void set();
	void setMode(Mode mode);
private:
	int screenPosX;
	int screenPosY;

	Mode mode;

	void setObserverMode();
};
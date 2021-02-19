#pragma once

#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Include/freeglut.h"

#include "input.h"
#include "window.h"


class Camera
{
public:
	Camera();
	const void set();
private:
	int screenPosX;
	int screenPosY;
};


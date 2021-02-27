#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <cassert>


char* concatenatedCharPtr(const char* a, const char* b);


float toRadian(float degrees);


struct Extrema {
public:
	float min, max;

	Extrema(float min, float max);

	float spread() const;
};


struct Measurements {
	float height, width, depth;

	Measurements(float height, float width, float depth);
};


bool toggleFlag(bool flag);
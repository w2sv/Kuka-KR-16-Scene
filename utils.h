#pragma once

#include <cmath>
#include <cassert>

#include "Include/freeglut.h"

struct Vector3 {
public:
	GLfloat x, y, z;

	Vector3(GLfloat x, GLfloat y, GLfloat z);
};

struct Color {
public:
	GLfloat r, g, b, o;

	Color(double r, double g, double b, double o = 1.0);
	explicit Color(int r, int g, int b, double o = 1.0);

	const void render();
};

struct CoordinateExtrema {
public:
	float min, max;

	CoordinateExtrema(float min, float max);
	CoordinateExtrema(float absValue);

	const float spread();
};

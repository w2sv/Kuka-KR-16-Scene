#pragma once

#include "Include/freeglut.h"

struct Vector3 {
public:
	GLfloat x, y, z;

	Vector3(GLfloat x, GLfloat y, GLfloat z);
};

struct Color {
public:
	GLfloat r, g, b, o;

	Color(GLfloat r, GLfloat g, GLfloat b, GLfloat o = 1.0);

	const void render();
};

struct CoordinateExtrema {
public:
	float min, max;

	CoordinateExtrema(float min, float max);
	CoordinateExtrema(float absValue);
};
